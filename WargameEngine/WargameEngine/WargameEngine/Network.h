#include "NetSocket.h"
#include <vector>
#include <memory>
#include <map>
#include <string>
#include "model\ObjectInterface.h"
#include "controller\CommandHandler.h"
#include <functional>
#define callback(x) std::function<void()>(x)

class CNetwork
{
public:
	static std::weak_ptr<CNetwork> GetInstance();
	static void FreeInstance();
	void Host(unsigned int port = 0);
	void Client(const char * ip, unsigned short port = 0);
	void Update();
	bool IsHost() const { return m_host; }
	void Stop();
	void SendState();
	void SendMessag(std::string const& message);
	void SendAction(ICommand* command, bool execute);
	bool IsConnected() { return m_socket.get() != NULL; }
	unsigned int GetAddress(std::shared_ptr<IObject> obj);
	std::shared_ptr<IObject> GetObject(unsigned int address);
	void AddAddressLocal(std::shared_ptr<IObject> obj);
	void AddAddress(std::shared_ptr<IObject> obj, unsigned int address);
	void SetStateRecievedCallback(callback(onStateRecieved));
	void SetStringRecievedCallback(std::function<void(const char*)> onStringRecieved);
	void CallStateRecievedCallback();
private:
	CNetwork() :m_host(true), m_netData(NULL), m_netRecievedSize(0), m_netTotalSize(0) {}
	static std::shared_ptr<CNetwork> m_instance;
	std::unique_ptr<CNetSocket> m_socket;
	std::vector<CNetSocket> m_childs;
	std::map<unsigned int, std::shared_ptr<IObject>> m_translator;
	bool m_host;
	int m_netRecievedSize;
	int m_netTotalSize;
	char * m_netData;
	callback(m_stateRecievedCallback);
	std::function<void(const char*)>m_stringRecievedCallback;
};