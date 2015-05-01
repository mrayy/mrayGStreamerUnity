

#include "stdafx.h"
#include "Win32UDPClient.h"

#include "Win32Network.h"

#ifdef UNIX
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#ifdef HAVE_IPV6
#include <resolv.h>
#endif // HAVE_IPV6
#endif // UNIX

#ifdef WIN32
#include <winsock2.h>
#include <WS2tcpip.h>  // for extra socket options
#endif  // WIN32

#include <stdio.h>
#include <string.h>

// Hack for using with NRL IPSEC implementation
#ifdef NETSEC
#include <net/security.h>
extern void *netsec_request;
extern int netsec_requestlen;
#endif // NETSEC

#define DMSG(x,y) printf(y);
#define TRACE(x,y)printf(x,y);
#define ASSERT(x) FATAL_ERROR(!x,#x)


namespace mray
{
namespace network
{
	Win32UDPClient::Win32UDPClient()
		: handle(INVALID_SOCKET),
		domain(PF_INET),
		port(0), connected(false), owner(NULL),
		recv_handler(NULL), install_func(NULL), install_data(NULL),
		user_data(NULL)
	{

	}

	Win32UDPClient::~Win32UDPClient()
	{
		Close();
	}

	// WIN32 needs the address type determine IPv6 _or_ IPv4 socket domain
	// Note: WIN32 can't do IPv4 on an IPV6 socket!
	UDPClientError Win32UDPClient::Open(unsigned short thePort)
	{
#ifdef WIN32
		// Startup WinSock
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD( 2, 2 );
		int err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) return UDP_SOCKET_ERROR_OPEN;

		// Since we're doing QOS, we need find a QoS-capable UDP service provider
		WSAPROTOCOL_INFO* infoPtr = NULL;
		WSAPROTOCOL_INFO* protocolInfo = NULL;
		DWORD buflen = 0;
		// Query to properly size protocolInfo buffer
		WSAEnumProtocols(NULL, protocolInfo, &buflen);
		if (buflen)
		{
			// Enumerate, try to find QOS-capable UDP, and create a socket
			if ((protocolInfo = (WSAPROTOCOL_INFO*) new char[buflen]))
			{
				int count = WSAEnumProtocols(NULL, protocolInfo, &buflen);
				if (SOCKET_ERROR != count)
				{
					for (int i = 0; i < count; i++)
					{
						if ((IPPROTO_UDP == protocolInfo[i].iProtocol) &&
							(XP1_QOS_SUPPORTED & protocolInfo[i].dwServiceFlags1))
						{
							infoPtr = protocolInfo + i;
						}
					}
				}
				else
				{
					DMSG(0, "Win32UDPClient: WSAEnumProtocols() error2!\n");
					return UDP_SOCKET_ERROR_OPEN;
				}
			}
			else
			{
				DMSG(0, "Win32UDPClient: Error allocating memory!\n");
				return UDP_SOCKET_ERROR_OPEN;
			}		
		}
		else
		{
			DMSG(0, "Win32UDPClient::Open() WSAEnumProtocols() error1!\n");			
		}
		int family;
#ifdef HAVE_IPV6
		if(type == IPv6)
		{
			domain = PF_INET6;
			family = AF_INET6;
		}
		else
#endif // HAVE_IPV6
		{
			domain = PF_INET;
			family = AF_INET;
		}    

		// Use WSASocket() to open right kind of socket
		// (Just a regular socket if infoPtr == NULL
		handle = WSASocket(family, SOCK_DGRAM, 0,
			infoPtr, 0, 
			WSA_FLAG_OVERLAPPED | 
			WSA_FLAG_MULTIPOINT_C_LEAF);
		if (protocolInfo) delete[] protocolInfo;
		if (INVALID_SOCKET == handle)
		{
			perror("Win32UDPClient: WSASocket() error");
			return UDP_SOCKET_ERROR_OPEN;
		}
#else
#ifdef HAVE_IPV6
		if (IsIPv6Capable())
		{
			domain = PF_INET6;
			DMSG(0, "IPv6 support enabled ...\n");
		}
		else
#endif
		{
			domain = PF_INET;
			DMSG(0, "IPv4 support only ...\n");
		}
		if (INVALID_SOCKET == (handle = socket(domain, SOCK_DGRAM, 0)))
		{
			perror("Win32UDPClient: socket() error");
			return UDP_SOCKET_ERROR_OPEN;
		}    
#endif  // if/else WIN32

#ifdef NETSEC
		if (net_security_setrequest(handle, 0, netsec_request, netsec_requestlen))
		{
			perror("Win32UDPClient: net_security_setrequest() error");
			close(handle);
			handle = INVALID_SOCKET;
			return UDP_SOCKET_ERROR_OPEN;
		}
#endif // NETSEC

#ifdef UNIX
		if(-1 == fcntl(handle, F_SETFD, FD_CLOEXEC))
			perror("Win32UDPClient: fcntl(FD_CLOEXEC) error");
#endif // UNIX

#ifdef __socklen_t_defined
		socklen_t sockLen;
#else
		int sockLen;
#endif // __socklen_t_defined

#ifdef HAVE_IPV6
		struct sockaddr_storage socketAddr;
		if (PF_INET6 == domain)
		{
			memset((char *)&socketAddr, 0, sizeof(struct sockaddr_in6));	
			((struct sockaddr_in6 *)&socketAddr)->sin6_family = AF_INET6;
			((struct sockaddr_in6 *)&socketAddr)->sin6_addr = in6addr_any;
			((struct sockaddr_in6 *)&socketAddr)->sin6_port = htons(thePort);
			((struct sockaddr_in6 *)&socketAddr)->sin6_flowinfo = 0;
			sockLen = sizeof(struct sockaddr_in6);
		}
		else
		{
			memset((char *)&socketAddr, 0, sizeof(struct sockaddr));	
			((struct sockaddr_in *)&socketAddr)->sin_family = AF_INET;
			((struct sockaddr_in *)&socketAddr)->sin_addr.s_addr = htonl(INADDR_ANY);
			((struct sockaddr_in *)&socketAddr)->sin_port = htons(thePort);
			sockLen = sizeof(struct sockaddr_in);
		}
#else
		struct sockaddr socketAddr;
		memset((char *)&socketAddr, 0, sizeof(struct sockaddr));	
		((struct sockaddr_in *)&socketAddr)->sin_family = AF_INET;
		((struct sockaddr_in *)&socketAddr)->sin_addr.s_addr = htonl(INADDR_ANY);
		((struct sockaddr_in *)&socketAddr)->sin_port = htons(thePort);
		sockLen = sizeof(struct sockaddr_in);
#endif  //  if/else HAVE_IPV6

		// Bind the socket to the given port	 
		if (bind(handle, (struct sockaddr*)&socketAddr, sockLen) < 0)
		{
			perror("Win32UDPClient: bind() error");
#ifdef UNIX
			close(handle);
#endif // UNIX
#ifdef WIN32
			closesocket(handle);
#endif // WIN32
			return UDP_SOCKET_ERROR_BIND;
		}

		// Get socket name so we know our port number  
		sockLen = sizeof(socketAddr);
		if (getsockname(handle, (struct sockaddr*)&socketAddr, &sockLen) < 0) 
		{    
			perror("Win32UDPClient: getsockname() error");
#ifdef UNIX
			close(handle);
#endif // UNIX
#ifdef WIN32
			closesocket(handle);
#endif // WIN32
			return UDP_SOCKET_ERROR_NAME;
		}

		switch(((struct sockaddr*)&socketAddr)->sa_family)
		{
		case AF_INET:    
			port = ntohs(((struct sockaddr_in*)&socketAddr)->sin_port);
			break;
#ifdef HAVE_IPV6	    
		case AF_INET6:
			port = ntohs(((struct sockaddr_in6*)&socketAddr)->sin6_port);
			break;
#endif // HAVE_IPV6	    
		default:
			perror("Win32UDPClient: getsockname() returned unknown address type");
#ifdef UNIX
			close(handle);
#endif // UNIX
#ifdef WIN32
			closesocket(handle);
#endif // WIN32
			return UDP_SOCKET_ERROR_NAME;;
		}

		if (install_func)
			install_func(UDP_SOCKET_INSTALL, this, install_data);
		//#endif // if/else WIN32
		return UDP_SOCKET_ERROR_NONE;  // no error

	}  // end Open()


	void Win32UDPClient::Close()
	{
		if (IsOpen()) 
		{
			if (connected) Disconnect();
			if (install_func) 
				install_func(UDP_SOCKET_REMOVE, this, install_data);
#ifdef UNIX
			close(handle);
#endif // UNIX
#ifdef WIN32
			closesocket(handle);
			WSACleanup();
#endif // WIN32
		}
		handle = INVALID_SOCKET;
		port = 0;
	}  // end Close() 



	bool Win32UDPClient::Connect(const NetAddress& theAddress)
	{
#ifdef WIN32
		/*
		sockaddr_in addr;
		Win32Network::CreateAddress(theAddress,addr);

		int result = WSAConnect(handle,
			(struct sockaddr*)&addr,
			sizeof(struct sockaddr),
			NULL, NULL, NULL, NULL);*/
		bool result=Win32Network::inner_connect(handle,theAddress);
		if (false == result)
		{
			TRACE("Win32UDPClient::Connect()  error: (%d)\n",
				WSAGetLastError());
			Disconnect();
			return false;
		}
		else
		{
			connected = true;
			return true;
		}
#else
		return false;
#endif // if/else WIN32
	}  // end bool Win32UDPClient::Connect()

	void Win32UDPClient::Disconnect()
	{
#ifdef WIN32
		struct sockaddr nullAddr;
		memset(&nullAddr, 0 , sizeof(struct sockaddr));
		int result = WSAConnect(handle,
			&nullAddr,
			sizeof(struct sockaddr),
			NULL, NULL, NULL, NULL);
		if (SOCKET_ERROR == result)
		{
			TRACE("Win32UDPClient::Disconnect()  error: (%d)\n",
				WSAGetLastError());
		}
#endif  // WIN32
		connected = false;
	}  // end Win32UDPClient::Disconnect()

	UDPClientError Win32UDPClient::SendTo(const NetAddress* destAddr, const char* buffer, 
		unsigned int len, unsigned int* outlen)
	{
		if (connected)
		{
			int l = send(handle, (const char*)buffer, (size_t)len, 0);
			if (outlen)
				*outlen = l;
			if (l< 0)
			{
				perror("Win32UDPClient::SendTo(): send() error:");
#ifdef WIN32
				TRACE("Win32UDPClient::SendTo() error (%d)\n", WSAGetLastError());
#endif // WIN32
				return UDP_SOCKET_ERROR_SEND;
			}
			else
			{
				return UDP_SOCKET_ERROR_NONE;
			}
		}
		else
		{
			sockaddr_in addr;
			sockaddr_in *addrPtr=0;
			if(destAddr)
			{
				Win32Network::CreateAddress(*destAddr,addr);
				addrPtr=&addr;
			}
			int l = sendto(handle, buffer, (size_t)len, 0,
				(struct sockaddr*)addrPtr,
				sizeof(addr));
			if (outlen)
				*outlen = l;
			if (l < 0)
			{
#ifdef WIN32
				TRACE("Win32UDPClient::SendTo() error (%d)\n", WSAGetLastError());
#else
				perror("Win32UDPClient::SendTo(): sendto() error:");
#endif // WIN32
				return UDP_SOCKET_ERROR_SEND;
			}
			else
			{
				return UDP_SOCKET_ERROR_NONE;
			}
		}
	}  // end Win32UDPClient::SendTo()


	void Win32UDPClient::SetNonBlocking(bool b)
	{
		Win32Network::setSocketOption(handle, network::ESocketOption::ESO_NONBLOCK, true);
	}
	bool Win32UDPClient::WaitForData(int sec, int usec)
	{
		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(handle, &readSet);
		struct timeval tv,*tvp=0;
		tv.tv_sec = sec;
		tv.tv_usec = usec;
		if (sec != -1 || usec != -1)
			tvp = &tv;
		if (select(handle + 1, &readSet, 0, 0, tvp) == SOCKET_ERROR)
			return false;
		FD_ISSET(handle, &readSet);
		return true;
	}
	UDPClientError Win32UDPClient::RecvFrom(char*            buffer, 
		unsigned int*    buflen, 
		NetAddress*  sourceAddr, int flags)
	{
		int result=Win32Network::inner_receivefrom(handle,buffer,(int)*buflen,sourceAddr,flags);
		if (result < 0)
		{
			*buflen=0;
			return UDP_SOCKET_ERROR_RECV;
		}
		else
		{
			*buflen = result;
			return UDP_SOCKET_ERROR_NONE;
		}
#if 0
		sockaddr_in addr;
		Win32Network::CreateAddress(*sourceAddr,addr);

		int len = sizeof(addr);
#ifdef __socklen_t_defined
		int result = recvfrom(handle, buffer, (size_t)*buflen, 0, 
			(struct sockaddr*)&sourceAddr->addr, 
			(socklen_t*)&sourceAddr->len);
#else

		int result = recvfrom(handle, buffer, (size_t)*buflen, 0, 
			(struct sockaddr*)&addr, 
			(int*)&len);
#endif // __socklen_t_defined
		if (result < 0)
		{
#ifdef UNIX
			if (errno != EINTR)
				perror("Win32UDPClient: recvfrom() error");
#endif // UNIX
			*buflen = 0;
			return UDP_SOCKET_ERROR_RECV;
		}
		else
		{
			*buflen = result;
			/*
			switch(((struct sockaddr *)&addr)->sa_family)
			{
			case AF_INET:
				sourceAddr->type = IPv4;
				sourceAddr->len = 4;  // 4 bytes of IPv4 address
				break;

			case AF_INET6:
				sourceAddr->type = IPv6;
				sourceAddr->len = 16;  // 16 bytes of IPv6 address
				break;

			default:
				DMSG(0, "Win32UDPClient::RecvFrom() Unsupported address type!\n");
				return UDP_SOCKET_ERROR_RECV;
			}*/
			return UDP_SOCKET_ERROR_NONE;
		}
#endif
	}  // end Win32UDPClient::RecvFrom()

	UDPClientError Win32UDPClient::GetAvailableBytes(unsigned int* len)
	{
		u_long bytes = 0;
		
		int result = ioctlsocket(handle, FIONREAD, &bytes);
		if (result != 0)
		{
			*len = 0;
			return UDP_SOCKET_ERROR_RECV;
		}
		else
		{
			*len = bytes;
			return UDP_SOCKET_ERROR_NONE;
		}
	}


	void Win32UDPClient::OnReceive()
	{
		(owner->*recv_handler)(this);
	}
	UDPClientError Win32UDPClient::JoinGroup(const NetAddress* theAddress, 
		unsigned char         ttl)
	{
#ifdef WIN32
		sockaddr_in addr;
		Win32Network::CreateAddress(*theAddress,addr);
		SOCKET result = WSAJoinLeaf(handle,    
			((struct sockaddr*)&addr),
			sizeof(struct sockaddr),
			NULL, NULL,
			NULL, NULL,
			JL_BOTH);
		if (INVALID_SOCKET == result)
		{
			TRACE("WSAJoinLeaf() error: %d\n", WSAGetLastError());
			return UDP_SOCKET_ERROR_MCAST;
		}
#else
		int result;
#ifdef HAVE_IPV6
		if  (IPv6 == theAddress->Type())
		{
			if (IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6*)&theAddress->addr)->sin6_addr))
			{
				struct ip_mreq mreq;
				mreq.imr_multiaddr.s_addr = 
					IN6_V4MAPPED_ADDR(&(((struct sockaddr_in6*)&theAddress->addr)->sin6_addr));
				mreq.imr_interface.s_addr = INADDR_ANY;    	
				result = setsockopt(handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
			}
			else
			{
				struct ipv6_mreq mreq;
				mreq.ipv6mr_multiaddr = ((struct sockaddr_in6*)&theAddress->addr)->sin6_addr;
				mreq.ipv6mr_interface = 0;
				result = setsockopt(handle, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
			}
		}
		else
#endif // HAVE_IPV6
		{
			struct ip_mreq mreq;
			mreq.imr_multiaddr = ((struct sockaddr_in*)&theAddress->addr)->sin_addr;
			mreq.imr_interface.s_addr = INADDR_ANY;    	
			result = setsockopt(handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
		}
		if (result < 0)
		{ 
			perror("Win32UDPClient: Error joining multicast group");
			return UDP_SOCKET_ERROR_MCAST;
		}    
#endif // if/else WIN32

		UDPClientError err = SetTTL(ttl);
		if (UDP_SOCKET_ERROR_NONE != err) return err;

		return UDP_SOCKET_ERROR_NONE;
	}  // end Win32UDPClient::JoinGroup() 


	void Win32UDPClient::LeaveGroup(const NetAddress* theAddress)
	{    
		int result;
#ifdef HAVE_IPV6
		if (IPv6 == theAddress->Type())
		{
			if (IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6*)&theAddress->addr)->sin6_addr))
			{
				struct ip_mreq mreq;
				mreq.imr_multiaddr.s_addr = 
					IN6_V4MAPPED_ADDR(&(((struct sockaddr_in6*)&theAddress->addr)->sin6_addr));
				mreq.imr_interface.s_addr = INADDR_ANY; 
				result = setsockopt(handle, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
			}
			else
			{
				struct ipv6_mreq mreq;
				mreq.ipv6mr_multiaddr = ((struct sockaddr_in6*)&theAddress->addr)->sin6_addr;
				mreq.ipv6mr_interface = 0;
				result = setsockopt(handle, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
			}
		}
		else
#endif
		{
			sockaddr_in addr;
			Win32Network::CreateAddress(*theAddress,addr);
			struct ip_mreq mreq;
			mreq.imr_multiaddr = ((struct sockaddr_in*)&addr)->sin_addr;
			mreq.imr_interface.s_addr = INADDR_ANY; 
			result = setsockopt(handle, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
		}
		if (result < 0)
		{
			perror("Win32UDPClient: Error leaving multicast group");
		}
	}  // end Win32UDPClient::LeaveGroup() 

	UDPClientError Win32UDPClient::SetTTL(unsigned char ttl)
	{
#ifdef WIN32
		DWORD dwTTL = (DWORD)ttl; 
		DWORD dwBytesXfer;
		if (WSAIoctl(handle, SIO_MULTICAST_SCOPE, &dwTTL, sizeof(dwTTL),
			NULL, 0, &dwBytesXfer,	NULL, NULL))
#else
		int result;
#ifdef HAVE_IPV6
		if (PF_INET6 == domain)
		{
			unsigned int hops = (unsigned int) ttl;
			result = setsockopt(handle, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, 
				&hops, sizeof(hops));
		}
		else
#endif // HAVE_IPV6
		{
			result = setsockopt(handle, IPPROTO_IP, IP_MULTICAST_TTL, 
				(char*)&ttl, sizeof(ttl));
		}
		if (result < 0) 
#endif // if/else WIN32
		{ 
			perror("Win32UDPClient: setsockopt(IP_MULTICAST_TTL) error"); 
			return UDP_SOCKET_ERROR_MCAST;  /* Evidently non-mcast kernel */
		}
		else
		{   
			return UDP_SOCKET_ERROR_NONE;
		}
	}  // end Win32UDPClient::SetTTL()

	UDPClientError Win32UDPClient::SetTOS(unsigned char tos)
	{
#ifdef NEVER_EVER // (for older LINUX?)                  
		int precedence = IPTOS_PREC(tos);
		if (setsockopt(handle, SOL_SOCKET, SO_PRIORITY, (char*)&precedence, sizeof(precedence)) < 0)           
		{     
			perror("Win32UDPClient: setsockopt(SO_PRIORITY) error"); 
			return UDP_SOCKET_ERROR_TOS;
		}                          
		int tos_bits = IPTOS_TOS(tos);
		if (setsockopt(handle, SOL_IP, IP_TOS, (char*)&tos_bits, sizeof(tos_bits)) < 0) 
#else
		int theTOS = tos;
		int result;
#ifdef HAVE_IPV6
		if (PF_INET6 == domain)
			result = setsockopt(handle, IPPROTO_IPV6, IP_TOS, (char*)&theTOS, sizeof(theTOS)); 
		else 
#endif // HAVE_IPV6  
			result =  setsockopt(handle, IPPROTO_IP, IP_TOS, (char*)&theTOS, sizeof(theTOS));    
#endif  // if/else NEVER_EVER
		if (result < 0)
		{               
			perror("Win32UDPClient: setsockopt(IP_TOS) error");
			return UDP_SOCKET_ERROR_TOS;
		}
		return UDP_SOCKET_ERROR_NONE; 
	}  // end Win32UDPClient::SetTOS()

	UDPClientError Win32UDPClient::SetLoopback(bool loopback)
	{
#ifdef WIN32
		DWORD dwLoop = loopback ? TRUE: FALSE;
		DWORD dwBytesXfer;
		if (WSAIoctl(handle, SIO_MULTIPOINT_LOOPBACK , &dwLoop, sizeof(dwLoop),
			NULL, 0, &dwBytesXfer,	NULL, NULL))
#else 
		int result;
		char loop = loopback ? 1 : 0;
#ifdef HAVE_IPV6
		if (PF_INET6 == domain)
			result = setsockopt(handle, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, 
			(char*)&loop, sizeof(loop));
		else
#endif // HAVE_IPV6
			result = setsockopt(handle, IPPROTO_IP, IP_MULTICAST_LOOP, 
			(char*)&loop, sizeof(loop));
		if (result < 0)
#endif // if/else WIN32
		{
			perror("Win32UDPClient: setsockopt(IP_MULTICAST_LOOP) error"); // let this error slide
		} 
		return UDP_SOCKET_ERROR_NONE;
	}  // end Win32UDPClient::SetLoopback() 

	UDPClientError Win32UDPClient::SetMulticastInterface(unsigned long interfaceAddress)
	{
		if (interfaceAddress != INADDR_ANY)
		{
			struct in_addr localAddr;
			localAddr.s_addr = htonl(interfaceAddress);
			int result;
#ifdef HAVE_IPV6
			if (PF_INET6 == domain)  
				result = setsockopt(handle, IPPROTO_IPV6, IP_MULTICAST_IF, (char*)&localAddr, 
				sizeof(localAddr));
			else 
#endif // HAVE_IPV6     
				result = setsockopt(handle, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localAddr, 
				sizeof(localAddr));
			if (result < 0)
			{ 
				perror("Win32UDPClient: setsockopt(IP_MULTICAST_IF) error");
				return UDP_SOCKET_ERROR_IFACE;
			}      	
		}     
		return UDP_SOCKET_ERROR_NONE;
	}  // end Win32UDPClient::SetMulticastInterface()

	UDPClientError Win32UDPClient::SetReuse(bool state)
	{
#if defined(SO_REUSEADDR) || defined(SO_REUSEPORT)
		UDPClientError err = UDP_SOCKET_ERROR_NONE;
#else
		return UDP_SOCKET_ERROR_REUSE;
#endif
		int reuse;
		if (state)
			reuse = 1;
		else
			reuse = 0;
#ifdef SO_REUSEADDR
#ifdef WIN32
		BOOL i_reuse = (BOOL) reuse;
		if (setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, (char*)&i_reuse, sizeof(i_reuse)) < 0)
#else
		if (setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0)
#endif // if/else WIN32
		{
			perror("Win32UDPClient: setsockopt(REUSE_ADDR)");
			err = UDP_SOCKET_ERROR_REUSE;
		}
#endif // SO_REUSEADDR            

#ifdef SO_REUSEPORT  // not defined on Linux for some reason?
#ifdef WIN32
		BOOL i_reuse = (BOOL) reuse;
		if (setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, (char*)&i_reuse, sizeof(i_reuse)) < 0)
#else
		if (setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, (char*)&reuse, sizeof(reuse)) < 0)
#endif // if/else WIN32
		{
			perror("Win32UDPClient: setsockopt(REUSE_ADDR)");
			err = UDP_SOCKET_ERROR_REUSE;
		}
#endif // SO_REUSEPORT
		return err;
	}  // end UDPClientError::SetReuse()

#ifdef HAVE_IPV6
	bool Win32UDPClient::IsIPv6Capable()
	{
#ifdef WIN32
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(2, 0);
		int err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) return false;
		SOCKET handle = socket(AF_INET6, SOCK_DGRAM, 0);
		closesocket(handle);
		WSACleanup();
		if(INVALID_SOCKET == handle)
			return false;
		else
			return true;
#else
		if (0 == (_res.options & RES_INIT)) res_init();
		if (_res.options & RES_USE_INET6)
			return true;
		else
			return false;
#endif
	}
#endif // HAVE_IPV6
}
}

