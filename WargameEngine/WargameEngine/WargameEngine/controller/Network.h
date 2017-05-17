#pragma once
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <functional>
#include "../INetSocket.h"

namespace wargameEngine
{
namespace model
{
class IObject;
class IModel;
}
namespace controller
{
class IStateManager;
class CommandHandler;
class ICommand;
typedef std::function<std::unique_ptr<INetSocket>()> SocketFactory;

class Network
{
	typedef std::function<void()> OnStateRecievedHandler;
	typedef std::function<void(std::wstring const&)> OnStringReceivedHandler;
public:
	Network(IStateManager & stateManager, CommandHandler & commandHandler, model::IModel & model, SocketFactory const& socketFactory);
	void Host(unsigned short port = 0);
	void Client(const char * ip, unsigned short port = 0);
	void Update();
	bool IsHost() const;
	void Stop();
	void SendState();
	void SendMessage(std::wstring const& message);
	void SendAction(ICommand const& command);
	bool IsConnected();
	void AddAddressLocal(std::shared_ptr<model::IObject> const& obj);
	void AddAddress(std::shared_ptr<model::IObject> const& obj, void* address);
	void SetStateRecievedCallback(OnStateRecievedHandler const& onStateRecieved);
	void SetStringRecievedCallback(OnStringReceivedHandler const& onStringRecieved);
	void CallStateRecievedCallback();
private:
	void* GetAddress(void* obj);
	SocketFactory m_socketFactory;
	std::unique_ptr<INetSocket> m_socket;
	std::map<void*, void*> m_translator;
	bool m_host;
	size_t m_netRecievedSize;
	size_t m_netTotalSize;
	std::vector<char> m_netData;
	OnStateRecievedHandler m_stateRecievedCallback;
	OnStringReceivedHandler m_stringRecievedCallback;
	IStateManager & m_stateManager;
	CommandHandler & m_commandHandler;
	model::IModel & m_model;
};
}
}