#include "NetSocket.h"
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "LogWriter.h"

void LogError()
{
	int wsError = WSAGetLastError();
	switch (wsError)
	{
	case WSANOTINITIALISED:   CLogWriter::WriteLine("Net Error. Either the application has not called WSAStartup, or WSAStartup failed.");
		break;
	case WSAENETDOWN:      CLogWriter::WriteLine("Net Error. A socket operation encountered a dead network.");
		break;
	case WSAEAFNOSUPPORT:    CLogWriter::WriteLine("Net Error. An address incompatible with the requested protocol was used.");
		break;
	case WSAEINPROGRESS:    CLogWriter::WriteLine("Net Error. A blocking operation is currently executing.");
		break;
	case WSAEMFILE:      CLogWriter::WriteLine("Net Error. Too many open sockets.");
		break;
	case WSAENOBUFS:      CLogWriter::WriteLine("Net Error. An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.");
		break;
	case WSAEPROTONOSUPPORT:  CLogWriter::WriteLine("Net Error. The requested protocol has not been configured into the system, or no implementation for it exists.");
		break;
	case WSAEPROTOTYPE:    CLogWriter::WriteLine("Net Error. A protocol was specified in the socket function call that does not support the semantics of the socket type requested.");
		break;
	case WSAESOCKTNOSUPPORT:  CLogWriter::WriteLine("Net Error. The support for the specified socket type does not exist in this address family.");
		break;
	case WSAEINTR:      CLogWriter::WriteLine("Net Error. A blocking operation was interrupted by a call to WSACancelBlockingCall.");
		break;
	case WSAENOTSOCK:    CLogWriter::WriteLine("Net Error. An operation was attempted on something that is not a socket.");
		break;
	case WSAEWOULDBLOCK:    CLogWriter::WriteLine("Net Error. A non-blocking socket operation could not be completed immediately.");
		break;
	case WSAEFAULT:    CLogWriter::WriteLine("Net Error. The system detected an invalid pointer address in attempting to use a pointer argument in a call.");
		break;
	case WSAEINVAL:    CLogWriter::WriteLine("Net Error. An invalid argument was supplied.");
		break;
	case WSAEISCONN:    CLogWriter::WriteLine("Net Error. A connect request was made on an already connected socket.");
		break;
	case WSAENETRESET:    CLogWriter::WriteLine("Net Error. The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.");
		break;
	case WSAEOPNOTSUPP:    CLogWriter::WriteLine("Net Error. The attempted operation is not supported for the type of object referenced.");
		break;
	case WSAESHUTDOWN:    CLogWriter::WriteLine("Net Error. A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call.");
		break;
	case WSAEMSGSIZE:    CLogWriter::WriteLine("Net Error. A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself.");
		break;
	case WSAETIMEDOUT:    CLogWriter::WriteLine("Net Error. A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond.");
		break;
	case WSAECONNRESET:    CLogWriter::WriteLine("Net Error. An existing connection was forcibly closed by the remote host.");
		break;
	case WSAEACCES:    CLogWriter::WriteLine("Net Error. An attempt was made to access a socket in a way forbidden by its access permissions.");
		break;
	case WSAENOTCONN:    CLogWriter::WriteLine("Net Error. A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied.");
		break;
	case WSAEHOSTUNREACH:    CLogWriter::WriteLine("Net Error. A socket operation was attempted to an unreachable host.");
		break;
	case WSAECONNABORTED:    CLogWriter::WriteLine("Net Error. An established connection was aborted by the software in your host machine.");
		break;
	case WSAEADDRNOTAVAIL:    CLogWriter::WriteLine("Net Error. The requested address is not valid in its context.");
		break;
	case WSAEDESTADDRREQ:    CLogWriter::WriteLine("Net Error. A required address was omitted from an operation on a socket.");
		break;
	case WSAENETUNREACH:    CLogWriter::WriteLine("Net Error. A socket operation was attempted to an unreachable network.");
		break;
	case WSAEADDRINUSE:    CLogWriter::WriteLine("Net Error. Only one usage of each socket address (protocol/network address/port) is normally permitted.");
		break;
	default:        CLogWriter::WriteLine("Net Error. Unknown error.");// такая ситуация не должна возникать вообще
		break;
	}
}

CNetSocket::CNetSocket(unsigned short port)//Server
{
	if(!InitSocket()) return;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)   // winsock2.h: #define INVALID_SOCKET (SOCKET)(~0)
	{
		LogError();
		return;
	}
	m_sockAddr = new struct sockaddr_in;
	memset(m_sockAddr, 0, sizeof (struct sockaddr_in));
	if (!ChangeAddress(port)) CLogWriter::WriteLine("Net error. Cannot bind to port");
	listen(m_socket, 10);
	CLogWriter::WriteLine("Net OK. Host is up and running.");
	sockaddr_in addr;
	int size = sizeof(struct sockaddr_in);
	SOCKET socket = accept(m_socket, (struct sockaddr *)&addr, &size);
	if (socket == INVALID_SOCKET)
	{
		LogError();
		return;
	}
	closesocket(m_socket);
	m_socket = socket;
	unsigned long iMode = 1UL;
	int error = ioctlsocket(m_socket, FIONBIO, &iMode);
	if (error == SOCKET_ERROR)
	{
		LogError();
		return;
	}
	char text[255];
	sprintf(text, "Net OK. Client %s is accepted by the host.", inet_ntoa(addr.sin_addr));
	CLogWriter::WriteLine(text);
}

CNetSocket::CNetSocket(const char * ip, unsigned short port)//Client
{
	if (!InitSocket()) return;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)   // winsock2.h: #define INVALID_SOCKET (SOCKET)(~0)
	{
		LogError();
		return;
	}
	
	m_sockAddr = new struct sockaddr_in;
	memset(m_sockAddr, 0, sizeof (struct sockaddr_in));
	ChangeAddress(ip, port);
	CLogWriter::WriteLine("Net OK. Trying to connect host.");
	int error = connect(m_socket, (struct sockaddr *)m_sockAddr, sizeof(struct sockaddr_in));
	if (error == SOCKET_ERROR)
	{
		LogError();
		return;
	}
	unsigned long iMode = 1UL;
	error = ioctlsocket(m_socket, FIONBIO, &iMode);
	if (error == SOCKET_ERROR)
	{
		LogError();
		return;
	}
	CLogWriter::WriteLine("Net OK. Client is connected to the host.");
}

CNetSocket::CNetSocket(unsigned int socket, void* sockAddr) :m_socket(socket), m_sockAddr(sockAddr){}

bool CNetSocket::ChangeAddress(const char* ip, unsigned short port)
{
	struct sockaddr_in * addr = (struct sockaddr_in *)m_sockAddr;
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = inet_addr(ip);
	return true;
}

bool CNetSocket::ChangeAddress(unsigned short port)
{
	struct sockaddr_in * addr = (struct sockaddr_in *)m_sockAddr;
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = INADDR_ANY;
	int serror = bind(m_socket, (SOCKADDR*)addr, sizeof (*addr));
	if (serror == SOCKET_ERROR)
	{
		LogError();
		return false;
	}
	return true;
}

bool CNetSocket::InitSocket()
{
	WORD wVersion;          // запрашиваемая версия winsock-интерфейса
	WSADATA wsaData;        // сюда записываются данные о сокете
	wVersion = MAKEWORD(2, 2);      // задаем версию winsock

	int wsaInitError = WSAStartup(wVersion, &wsaData);    // инициализируем winsock
	if (wsaInitError != 0)
	{
		CLogWriter::WriteLine("Net Error. Error initalizing WSA.");
		return false;
	}
	else
		return true;// если инициализация прошла успешно, то пора создавать сокет
}

const char* CNetSocket::GetIP() const
{
	struct sockaddr_in name;
	int namelen = sizeof (struct sockaddr_in);
	int error = getsockname(m_socket, (struct sockaddr *) &name, &namelen);
	if (error == SOCKET_ERROR)
	{
		LogError();
	}
	return inet_ntoa(name.sin_addr);
}

unsigned short CNetSocket::GetPort() const
{
	struct sockaddr_in name;
	int namelen = sizeof (struct sockaddr_in);
	int error = getsockname(m_socket, (struct sockaddr *) &name, &namelen);
	if (error == SOCKET_ERROR)
	{
		LogError();
	}
	return ntohs(name.sin_port);
}

bool CNetSocket::SendData(const char * data, int len) const
{
	int count = send(m_socket, data, len, 0);
	if (count == SOCKET_ERROR)
	{
		LogError();
		return false;
	}
	return count == len;
}

int CNetSocket::RecieveData(char * data, int maxLength)
{
	int count = recv(m_socket, data, maxLength, 0);
	if (count == 0)
	{
		CLogWriter::WriteLine("Net OK. Connection is closed by the other side.");
	}
	if (count == SOCKET_ERROR)
	{
		int wsError = WSAGetLastError();
		if (wsError != WSAEWOULDBLOCK)
		{
			LogError();
		}
		if (wsError == WSAECONNRESET)
		{
			return 0;
		}
	}
	return count;
}

CNetSocket::~CNetSocket()
{
	int wsError = closesocket(m_socket);
	if (wsError) LogError();
	wsError = WSACleanup();
	if (wsError) LogError();
	delete (struct sockaddr_in*) m_sockAddr;
	CLogWriter::WriteLine("Net OK. Socket is closed.");
}