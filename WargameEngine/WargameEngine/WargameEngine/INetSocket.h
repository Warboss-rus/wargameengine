#pragma once
#include <string>

namespace wargameEngine
{
class INetSocket
{
public:
	virtual ~INetSocket() {}

	virtual void InitFromAnotherSocket(unsigned int socket, void* sockAddr) = 0;
	virtual void InitClient(const char* ip, unsigned short port = 0) = 0;
	virtual void InitHost(unsigned short port = 0) = 0;

	virtual bool SendData(const char* data, size_t len) const = 0;
	//Return -1 then error occurs, 0 then connection is closed by other side or number of bytes received.
	virtual int RecieveData(char* data, size_t maxLength) = 0;
};
}