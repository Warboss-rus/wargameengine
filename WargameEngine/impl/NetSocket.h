#pragma once
#include "../INetSocket.h"
#include <string>

class CNetSocket : public wargameEngine::INetSocket
{
public:
	void InitHost(unsigned short port = 0) override;
	void InitClient(const char* ip, unsigned short port = 0) override;
	void InitFromAnotherSocket(unsigned int socket, void* sockAddr) override;
	bool SendData(const char* data, size_t len) const override;
	int RecieveData(char* data, size_t maxLength) override;
	std::string GetIP() const;
	unsigned short GetPort() const;
	~CNetSocket();

private:
	bool InitSocket();
	bool ChangeAddress(const char* ip, unsigned short port = 0);
	bool ChangeAddress(unsigned short port = 0);
	int m_socket;
	void* m_sockAddr;
};