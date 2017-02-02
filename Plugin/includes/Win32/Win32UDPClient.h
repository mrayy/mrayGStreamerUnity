


/********************************************************************
	created:	2012/06/28
	created:	28:6:2012   17:27
	filename: 	d:\Development\mrayEngine\Engine\mrayNet\Win32UDPClient.h
	file path:	d:\Development\mrayEngine\Engine\mrayNet
	file base:	Win32UDPClient
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/


#ifndef __Win32UDPClient__
#define __Win32UDPClient__

#include "IUDPClient.h"
#include <winsock2.h>

namespace mray
{
namespace network
{
	class NetAddress;
	class CMsgBuffer;
#ifdef UNIX
	typedef int SocketHandle;
	const int INVALID_SOCKET = -1;
#endif // UNIX
#ifdef WIN32
	typedef SOCKET SocketHandle;
#endif // WIN32



	class  Win32UDPClient:public IUDPClient
	{
	public:
		// Construction
		Win32UDPClient();
		~Win32UDPClient();

		// Initialization
		void Init(UDPClientOwner*       theOwner, 
			UDPClientRecvHandler  recvHandler,
			UDPClientInstallFunc* installFunc,
			void*                 installData)
		{
			owner = theOwner;
			recv_handler = recvHandler;
			install_func = installFunc;
			install_data = installData;
		}

		UDPClientOwner* Owner() {return owner;}
		UDPClientRecvHandler RecvHandler() {return recv_handler;}

		void SetUserData(const void* userData) {user_data = userData;}
		const void* GetUserData() {return user_data;}

		// Control
		UDPClientError Open(unsigned short thePort = 0);
		void Close();
		bool Connect(const NetAddress& theAddress);
		void Disconnect();
		UDPClientError JoinGroup(const NetAddress* theAddress, 
			unsigned char ttl);
		void LeaveGroup(const NetAddress *theAddress);
		UDPClientError SetTTL(unsigned char ttl);
		UDPClientError SetLoopback(bool loopback);
		UDPClientError SetTOS(unsigned char tos);
		UDPClientError SetReuse(bool state);
		UDPClientError SetMulticastInterface(unsigned long interfaceAddress);

		//send data
		UDPClientError SendTo(const NetAddress* dest, const char* buffer, unsigned int len,unsigned int* outlen=0);
		UDPClientError RecvFrom(char* buffer, unsigned int* buflen, NetAddress* src,int flags);
		UDPClientError GetAvailableBytes(unsigned int* len);
		// Status
		bool IsOpen() {return (bool)(handle != INVALID_SOCKET);}        
		bool IsConnected() {return connected;}
		unsigned short Port() {return port;}
		SocketHandle Handle() {return handle;}

		virtual bool WaitForData(int sec, int usec);
		void SetNonBlocking(bool b);

		void OnReceive();
#ifdef HAVE_IPV6
		static bool Win32UDPClient::IsIPv6Capable();
#endif            
	private:
		// Members
		SocketHandle            handle;
		int                     domain;  // PF_INET or PF_INET6
		unsigned short          port;
		bool		            connected;
		UDPClientOwner*         owner;
		UDPClientRecvHandler    recv_handler;
		UDPClientInstallFunc*   install_func;
		const void*             install_data;
		const void*             user_data;

	};  // end class Win32UDPClient




}
}

#endif

