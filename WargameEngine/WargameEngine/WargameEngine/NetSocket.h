#pragma once
#include <string>

class CNetSocket
{
public:
	//Server
	CNetSocket(unsigned short port = 0);
	//Client
	CNetSocket(const char * ip, unsigned short port = 0);
	//serverToClient 
	CNetSocket(unsigned int socket, void* sockAddr);
	bool ChangeAddress(const char* ip, unsigned short port = 0);
	bool ChangeAddress(unsigned short port = 0);
	bool SendData(const char * data, int len) const;
	//Return -1 then error occures, 0 then connection is closed by other side or number of bytes recieved.
	int RecieveData(char* data, int maxLength);
	std::string GetIP() const;
	unsigned short GetPort() const;
	~CNetSocket();
private:
	bool InitSocket();
    unsigned int m_socket;
	void * m_sockAddr;
};