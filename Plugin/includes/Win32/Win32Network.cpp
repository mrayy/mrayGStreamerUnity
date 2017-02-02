
#include "stdafx.h"

#include "Win32Network.h"
#include "Win32UDPClient.h"
//#include <stdio.h>

#include <winsock2.h>


namespace mray{


namespace network{





Win32Network::Win32Network():SYNC_NUMBER(1234567)
{
	int error;
	WSAData wd;
	error=WSAStartup(MAKEWORD(2,2),&wd);

	
	if(error==SOCKET_ERROR){
		if(LOBYTE(wd.wVersion)!=2 || HIBYTE(wd.wVersion)!=0){
			WSACleanup();
			/////////error occourd
			
			return;
		}
	}

	DWORD buffSize=0;

	//Enumerate available protocols
	int numProto=WSAEnumProtocols(0,0,&buffSize);

	if(numProto!=SOCKET_ERROR && WSAGetLastError()!=WSAENOBUFS){
		WSACleanup();
		
		return;
	}

	LPWSAPROTOCOL_INFO selectedProto;

	selectedProto=(LPWSAPROTOCOL_INFO)malloc(buffSize);

	int *proto=new int[2];
	proto[0]=IPPROTO_TCP;
	proto[1]=IPPROTO_UDP;

	numProto=WSAEnumProtocols(proto,selectedProto,&buffSize);

	delete [] proto;
	proto=0; 

	free(selectedProto);
	selectedProto=0;


	 
}

Win32Network::~Win32Network(){
	WSACleanup();
}

int Win32Network::getSyncNumber(){
	return SYNC_NUMBER;
}
void Win32Network::CreateAddress(const NetAddress&addr,sockaddr_in& ret)
{
	ret.sin_family = AF_INET;
	ret.sin_addr.s_addr = addr.address;
	ret.sin_port = htons(addr.port);
}

IUDPClient* Win32Network::createUDPClient()
{
	return new Win32UDPClient();
}



void Win32Network::toSockaddr(const NetAddress &addr,sockaddr*sAddr){
	memset(sAddr,0,sizeof(sockaddr_in));
	((sockaddr_in*)sAddr)->sin_family=AF_INET;
	((sockaddr_in*)sAddr)->sin_port=htons(addr.port);
	((sockaddr_in*)sAddr)->sin_addr.s_addr=addr.address;

}

void Win32Network::toAddress(const sockaddr*sAddr,NetAddress &addr){
	addr.address=((sockaddr_in*)sAddr)->sin_addr.s_addr;
}


bool Win32Network::inner_getHostAddress(const char*name,NetAddress &addr){

	hostent*entry;
	entry=gethostbyname(name);
	if(!entry || entry->h_addrtype != AF_INET){
		u_long lhost=inet_addr(name);
		if(lhost==INADDR_NONE)
			return false;
		addr.address=lhost;
		return true;
	}

	addr.address=*(u_int*)entry->h_addr_list[0];

	return true;
}
bool Win32Network::getHostAddress(const char*name,NetAddress &addr)
{
	return inner_getHostAddress(name,addr);
}


bool Win32Network::getHostIP(const NetAddress &addr,char*name,int maxLen){
	char*hname=inet_ntoa(*(in_addr*)&addr.address);
	if(!hname)
		return false;
	strncpy(name,hname,maxLen);
	return true;
}
bool Win32Network::getHostName(const NetAddress &addr,char*name,int maxLen){

	in_addr in;
	hostent * entry;

	in.s_addr = addr.address;

	entry = gethostbyaddr ((char *) & in, sizeof (in_addr), AF_INET);
	if (entry == NULL)
		return getHostIP(addr, name, maxLen);

	strncpy (name, entry -> h_name, maxLen);

	return true;
}

bool Win32Network::setSocketOption(SOCKET s,ESocketOption option,int value){
	int result=SOCKET_ERROR;
	switch(option){
		case ESO_NONBLOCK:
			{
				u_long nb=value;
				result=ioctlsocket(s,FIONBIO,&nb);
			}break;

		case ESO_SOCKOPT_BROADCAST:
			result = setsockopt (s, SOL_SOCKET, SO_BROADCAST, (char *) & value, sizeof (int));
			break;

		case ESO_SOCKOPT_RCVBUF:
			result = setsockopt (s, SOL_SOCKET, SO_RCVBUF, (char *) & value, sizeof (int));
			break;

		case ESO_SOCKOPT_SNDBUF:
			result = setsockopt (s, SOL_SOCKET, SO_SNDBUF, (char *) & value, sizeof (int));
			break;

		default:
			break;
	}

	return result!=SOCKET_ERROR;
}

bool Win32Network::enableBroadcast(SOCKET s,bool enable){
	int on=enable;
	return setsockopt(s,SOL_SOCKET,SO_BROADCAST,(const char*)&on,sizeof(on))!=-1;
}


NetAddress Win32Network::getSockAddr(SOCKET s){
	sockaddr_in address;
	int len=sizeof(address);
	getsockname(s,(sockaddr*)&address,&len);

	static NetAddress addr;
	addr.address=address.sin_addr.s_addr;
	addr.port=ntohs(address.sin_port);
	return addr;
}

void Win32Network::closeSocket(SOCKET s){

	closesocket(s);

}

void Win32Network::shutdownSocket(SOCKET s){

	shutdown(s,SD_SEND);

}



ulong Win32Network::getLocalAddress(){
	ulong res=0;
	char name[128];
	int err=gethostname(name,sizeof(name));
	if(err==-1)return 0;

	hostent *hn=gethostbyname(name);
	if(hn)res=(ulong)hn->h_addr;

	return res;
}
int Win32Network::getSocketPort(SOCKET s){
	sockaddr_in sa;
	int len = sizeof(sa);
	if (getsockname(s, (sockaddr*)&sa, &len) != 0){
		return 0;
	}
	return ntohs(sa.sin_port);
}

int Win32Network::getLocalIPs(char ipLst[8][16]){
	char name[128];
	int err=gethostname(name,sizeof(name));
	if(err==-1)
		return 0;

	hostent *hn=gethostbyname(name);

	int cnt=0;
	for(cnt=0;hn->h_addr_list[ cnt ]!=0 && cnt<8;cnt++){
		
		struct in_addr addr;

		memcpy( &addr, hn->h_addr_list[cnt], sizeof( struct in_addr ) );
		strcpy( ipLst[cnt], inet_ntoa( addr ) );
	}

	return cnt;
}

bool Win32Network::getSocketAddress(SOCKET s,sockaddr_in&out){
	int addrlen=sizeof(sockaddr_in);

	int err=getsockname(s,(sockaddr *)&out,&addrlen);
	return (err==0);
}

bool Win32Network::inner_connect(SOCKET s,const NetAddress&addr){
	sockaddr_in hostAddr;
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_addr.s_addr = addr.address;
	hostAddr.sin_port = htons(addr.port);
	if(::connect(s,(struct sockaddr *)&hostAddr,sizeof(hostAddr))<0)
		return false;

	return true;
}

int Win32Network::inner_receivefrom(SOCKET s,void*data,int len,
					  NetAddress * fromAddress,int flags)
{


	DWORD  recvLength = 0;
	struct sockaddr_in sin;

	if (!fromAddress)
		recvLength = ::recv(s, (char*)data, len, flags);
	else
	{
		int sinLength = sizeof (struct sockaddr_in);
		sin.sin_family = AF_INET;

		sin.sin_addr.s_addr = 0;
		recvLength = ::recvfrom(s, (char*)data, len, flags,
			(struct sockaddr *) & sin, &sinLength);
	}
	if (recvLength == SOCKET_ERROR)
	{
		int err=WSAGetLastError ();
		if (err == WSAEWOULDBLOCK || err == WSAECONNRESET){

			return 0;
		}


		return -1;
	}

	if (flags & MSG_PARTIAL){

		return -1;
	}

	if (fromAddress != NULL)
	{
		fromAddress -> address = (u_int) sin.sin_addr.s_addr;
		fromAddress -> port = ntohs (sin.sin_port);
	}

	return (int) recvLength;
}
int Win32Network::wait(SOCKET s,uint timeout,int  condition){
	fd_set readSet, writeSet;
	struct timeval timeVal;
	int selectCount;

	int result=(int)ESW_NONE;
    
	timeVal.tv_sec = timeout / 1000;
	timeVal.tv_usec = (timeout % 1000) * 1000;
    
	FD_ZERO (& readSet);
	FD_ZERO (& writeSet);

	if ( condition & (int)ESW_SEND)
	  FD_SET (s, & writeSet);

	if (condition & (int)ESW_RECEIVE)
	  FD_SET (s, & readSet);

	selectCount = select (s+1, & readSet, & writeSet, NULL, & timeVal);

	if (selectCount < 0)
	  return -1;

	result = (int)ESW_NONE;

	if (selectCount == 0)
	  return result;

	if (FD_ISSET (s, & writeSet))
	  result |= (int)ESW_SEND;
    
	if (FD_ISSET (s, & readSet))
	  result |= (int)ESW_RECEIVE;

	return result;
}


}
}
