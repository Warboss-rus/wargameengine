#pragma once
#include <string>
#include "controller\INetSocket.h"

class CNetSocket : public INetSocket
{
public:
	virtual void InitHost(unsigned short port = 0) override;
	virtual void InitClient(const char * ip, unsigned short port = 0) override;
	virtual void InitFromAnotherSocket(unsigned int socket, void* sockAddr) override;
	virtual bool SendData(const char * data, size_t len) const override;
	virtual int RecieveData(char* data, size_t maxLength) override;
	std::string GetIP() const;
	unsigned short GetPort() const;
	~CNetSocket();
private:
	bool InitSocket();
	bool ChangeAddress(const char* ip, unsigned short port = 0);
	bool ChangeAddress(unsigned short port = 0);
    int m_socket;
	void * m_sockAddr;
};