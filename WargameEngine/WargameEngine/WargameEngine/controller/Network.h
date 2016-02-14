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
typedef std::function<std::unique_ptr<INetSocket>()> SocketFactory;

class CNetwork
{
public:
	CNetwork(IStateManager & stateManager, CCommandHandler & commandHandler, IGameModel & model, SocketFactory const& socketFactory);
	void Host(unsigned short port = 0);
	void Client(const char * ip, unsigned short port = 0);
	void Update();
	bool IsHost() const;
	void Stop();
	void SendState();
	void SendMessage(std::string const& message);
	void SendAction(std::vector<char> const& command, bool execute);
	bool IsConnected();
	std::shared_ptr<IObject> GetObject(unsigned int address);
	void AddAddressLocal(std::shared_ptr<IObject> obj);
	void AddAddress(std::shared_ptr<IObject> obj, unsigned int address);
	void SetStateRecievedCallback(std::function<void()> onStateRecieved);
	void SetStringRecievedCallback(std::function<void(const char*)> onStringRecieved);
	void CallStateRecievedCallback();
private:
	unsigned int GetAddress(std::shared_ptr<IObject> obj);
	unsigned int GetAddress(IObject* obj);
	SocketFactory m_socketFactory;
	std::unique_ptr<INetSocket> m_socket;
	std::map<uintptr_t, std::shared_ptr<IObject>> m_translator;
	bool m_host;
	int m_netRecievedSize;
	int m_netTotalSize;
	char * m_netData;
	std::function<void()> m_stateRecievedCallback;
	std::function<void(const char*)>m_stringRecievedCallback;
	IStateManager & m_stateManager;
	CCommandHandler & m_commandHandler;
	IGameModel & m_model;
};