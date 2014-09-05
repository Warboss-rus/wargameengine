#pragma once

class CNetSocket
{
public:
	CNetSocket(unsigned short port = 0);//Server
	CNetSocket(const char * ip, unsigned short port = 0);//Client
	CNetSocket(unsigned int socket, void* sockAddr);//serverToClient 
	bool ChangeAddress(const char* ip, unsigned short port = 0);
	bool ChangeAddress(unsigned short port = 0);
	bool SendData(const char * data, int len) const;
	int RecieveData(char* data, int maxLength);//Return -1 then error occures, 0 then connection is closed by other side or number of bytes recieved.
	const char* GetIP() const;
	unsigned short GetPort() const;
	~CNetSocket();
private:
	bool InitSocket();
    unsigned int m_socket;
	void * m_sockAddr;
};