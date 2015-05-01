



/********************************************************************
	created:	2012/06/28
	created:	28:6:2012   18:09
	filename: 	d:\Development\mrayEngine\Engine\mrayEngine\include\IUDPClient.h
	file path:	d:\Development\mrayEngine\Engine\mrayEngine\include
	file base:	IUDPClient
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/


#ifndef __IUDPClient__
#define __IUDPClient__

#include <string>

namespace mray
{
namespace network
{
	class NetAddress;
	// Possible errors returned by some Win32UDPClient calls
	typedef enum UDPClientError
	{
		UDP_SOCKET_ERROR_NONE = 0, 
		UDP_SOCKET_ERROR,         // generic error
		UDP_SOCKET_ERROR_OPEN,    // socket() error
		UDP_SOCKET_ERROR_BIND,    // bind() error
		UDP_SOCKET_ERROR_CONNECT, // connect() error
		UDP_SOCKET_ERROR_NAME,    // getsockname() error
		UDP_SOCKET_ERROR_SEND,    // sendto() error
		UDP_SOCKET_ERROR_RECV,    // recvfrom() error        
		UDP_SOCKET_ERROR_MCAST,   // join group error
		UDP_SOCKET_ERROR_TTL,     // mcast ttl error
		UDP_SOCKET_ERROR_IFACE,   // mcast iface error
		UDP_SOCKET_ERROR_TOS,     // ip tos error
		UDP_SOCKET_ERROR_LOOP,    // loopback error
		UDP_SOCKET_ERROR_REUSE,   // port/addr reuse error
		UDP_SOCKET_ERROR_SEC      // ipsec error
	} UDPClientError;


	// The ("Win32UDPClientOwner" class exists only for the derivation of 
	// other classes. Therefore, no constructor is defined.
	class UDPClientOwner {};
	typedef bool (UDPClientOwner::*UDPClientRecvHandler)(class IUDPClient* theSocket);

	// For socket read notification installation
	enum UDPClientCmd{UDP_SOCKET_INSTALL, UDP_SOCKET_REMOVE};
	
	typedef bool (UDPClientInstallFunc)(UDPClientCmd cmd, class IUDPClient* theSocket,const void* installData);


class IUDPClient
{
private:
protected:
public:
	IUDPClient(){}
	virtual~IUDPClient(){}

	// Initialization
	virtual void Init(UDPClientOwner*       theOwner, 
		UDPClientRecvHandler  recvHandler,
		UDPClientInstallFunc* installFunc,
		void*                 installData)=0;


	virtual UDPClientOwner* Owner()=0;
	virtual UDPClientRecvHandler RecvHandler()=0;

	virtual void SetUserData(const void* userData) =0;
	virtual const void* GetUserData() =0;

	// Control
	virtual UDPClientError Open(unsigned short thePort = 0)=0;
	virtual void Close() =0;
	virtual bool Connect(const NetAddress& theAddress) =0;
	virtual void Disconnect() =0;
	virtual UDPClientError JoinGroup(const NetAddress* theAddress, 
		unsigned char ttl) =0;
	virtual void LeaveGroup(const NetAddress *theAddress) =0;
	virtual UDPClientError SetTTL(unsigned char ttl) =0;
	virtual UDPClientError SetLoopback(bool loopback) =0;
	virtual UDPClientError SetTOS(unsigned char tos) =0;
	virtual UDPClientError SetReuse(bool state) =0;
	virtual UDPClientError SetMulticastInterface(unsigned long interfaceAddress) =0;

	virtual UDPClientError SendTo(const NetAddress* dest, const char* buffer, unsigned int len, unsigned int* outlen = 0) = 0;
	virtual UDPClientError RecvFrom(char* buffer, unsigned int* buflen, NetAddress* src,int flags) =0;

	//-1,-1 for infinite
	virtual bool WaitForData(int sec = -1, int usec = -1) = 0;
	virtual UDPClientError GetAvailableBytes(unsigned int* len)=0;

	virtual void SetNonBlocking(bool b)=0;

	// Status
	virtual bool IsOpen()  =0;
	virtual bool IsConnected()  =0;
	virtual unsigned short Port() =0;

	static std::string TranslateErrorMessage(UDPClientError e)
	{
		switch(e)
		{
		case UDP_SOCKET_ERROR_NONE:		return ("None");
		case UDP_SOCKET_ERROR:			return ("Generic Error");
		case UDP_SOCKET_ERROR_OPEN:		return ("Open Socket");
		case UDP_SOCKET_ERROR_BIND:		return ("Bind Socket");
		case UDP_SOCKET_ERROR_CONNECT:	return ("Connect");
		case UDP_SOCKET_ERROR_NAME:		return ("getsockname()");
		case UDP_SOCKET_ERROR_SEND:		return ("Send");
		case UDP_SOCKET_ERROR_RECV:		return ("Receive");
		case UDP_SOCKET_ERROR_MCAST:	return ("Join group error");
		case UDP_SOCKET_ERROR_TTL:		return ("MCAST Time To Live (TTL)");
		case UDP_SOCKET_ERROR_IFACE:	return ("MCAST iFace");
		case UDP_SOCKET_ERROR_TOS:		return ("ip tos");
		case UDP_SOCKET_ERROR_LOOP:		return ("loopback");
		case UDP_SOCKET_ERROR_REUSE:	return ("port/addr reuse");
		case UDP_SOCKET_ERROR_SEC:		return ("ipsec");
		}
		return ("");
	}
};


}
}

#endif
