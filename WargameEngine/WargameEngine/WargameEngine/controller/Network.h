#pragma once
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <functional>
#include "INetSocket.h"

class IStateManager;
class CCommandHandler;
class IObject;
class IGameModel;
class ICommand;
typedef std::function<std::unique_ptr<INetSocket>()> SocketFactory;

class CNetwork
{
	typedef std::function<void()> OnStateRecievedHandler;
	typedef std::function<void(std::string const&)> OnStringReceivedHandler;
public:
	CNetwork(IStateManager & stateManager, CCommandHandler & commandHandler, IGameModel & model, SocketFactory const& socketFactory);
	void Host(unsigned short port = 0);
	void Client(const char * ip, unsigned short port = 0);
	void Update();
	bool IsHost() const;
	void Stop();
	void SendState();
	void SendMessage(std::string const& message);
	void SendAction(ICommand const& command);
	bool IsConnected();
	void AddAddressLocal(std::shared_ptr<IObject> obj);
	void AddAddress(std::shared_ptr<IObject> obj, void* address);
	void SetStateRecievedCallback(OnStateRecievedHandler const& onStateRecieved);
	void SetStringRecievedCallback(OnStringReceivedHandler const& onStringRecieved);
	void CallStateRecievedCallback();
private:
	void* GetAddress(void* obj);
	SocketFactory m_socketFactory;
	std::unique_ptr<INetSocket> m_socket;
	std::map<void*, void*> m_translator;
	bool m_host;
	int m_netRecievedSize;
	int m_netTotalSize;
	char * m_netData;
	OnStateRecievedHandler m_stateRecievedCallback;
	OnStringReceivedHandler m_stringRecievedCallback;
	IStateManager & m_stateManager;
	CCommandHandler & m_commandHandler;
	IGameModel & m_model;
};