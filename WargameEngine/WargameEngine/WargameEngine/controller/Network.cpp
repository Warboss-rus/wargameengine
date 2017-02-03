#include "Network.h"
#include <string.h>
#include "../LogWriter.h"
#include "../model/GameModel.h"
#include "../model/Object.h"
#include "CommandHandler.h"
#include "IStateManager.h"
#include "INetSocket.h"
#include "../MemoryStream.h"
#include "ICommand.h"

CNetwork::CNetwork(IStateManager & stateManager, CCommandHandler & commandHandler, IGameModel & model, SocketFactory const& socketFactory)
	: m_socketFactory(socketFactory)
	, m_host(true)
	, m_netRecievedSize(0)
	, m_netTotalSize(0)
	, m_stateManager(stateManager)
	, m_commandHandler(commandHandler)
	, m_model(model)
{
}

void CNetwork::Host(unsigned short port)
{
	if (m_socket)
	{
		LogWriter::WriteLine("Net error. Already connected");
		return;
	}
	m_socket = m_socketFactory();
	m_socket->InitHost(port);
	m_host = true;
	SendState();
}

void CNetwork::Client(const char * ip, unsigned short port)
{
	if (m_socket)
	{
		LogWriter::WriteLine("Net error. Already connected");
		return;
	}
	m_socket = m_socketFactory();
	m_socket->InitClient(ip, port);
	m_host = false;
}

void CNetwork::Stop()
{
	m_socket.reset();
	m_host = true;
	m_netData.clear();
	m_netRecievedSize = 0;
	m_netTotalSize = 0;
}

void CNetwork::Update()
{
	if (!m_socket) return;
	int result;
	if (!m_netData.empty())
	{
		result = m_socket->RecieveData(m_netData.data() + m_netRecievedSize, m_netTotalSize - m_netRecievedSize);
		m_netRecievedSize += result;
	}
	else
	{
		char data[128];
		result = m_socket->RecieveData(data, 128);
		if (result > 0)
		{
			m_netTotalSize = CReadMemoryStream(data + 1).ReadSizeT();
			m_netData.resize(m_netTotalSize);
			m_netRecievedSize = result;
			memcpy(m_netData.data(), data, result);
		}
	}
	if (result == 0)
	{
		m_socket.reset();
	}
	if (!m_netData.empty() && m_netRecievedSize >= m_netTotalSize && m_netRecievedSize > 4)
	{
		CReadMemoryStream stream(m_netData.data());
		unsigned char type = stream.ReadByte();
		if (type == 0) //string
		{
			auto message = stream.ReadWString();
			if (m_stringRecievedCallback)
			{
				m_stringRecievedCallback(message);
			}
			LogWriter::WriteLine(L"String received:" + message);
		}
		else if (type == 1) //state
		{
			char state[30];
			sprintf(state, "State Received. Size=%zu.", m_netRecievedSize);
			LogWriter::WriteLine(state);
			m_stateManager.LoadState(stream);
			if (m_stateRecievedCallback) m_stateRecievedCallback();
		}
		else if (type == 2) //command
		{
			stream.ReadUnsigned();//skip size
			char command = m_netData[5];//read without moving forward
			switch (command)
			{
			case 1://DeleteObject, remove from translator
			{
				uint64_t address;
				memcpy(&address, m_netData.data() + 6, sizeof(uint64_t));
				void * addressPtr = reinterpret_cast<void*>(address);
				uint64_t translated = reinterpret_cast<uint64_t>(m_translator.at(addressPtr));
				memcpy(m_netData.data() + 6, &translated, sizeof(uint64_t));
				m_translator.erase(addressPtr);
				LogWriter::WriteLine("DeleteObject received");
			}break;
			case 2://MoveObject
			case 3://RotateObject
			case 4://ChangeProperty
			case 6://PlayAnimation
			case 7://GoTo
			{//replace address
				uint64_t address;
				memcpy(&address, m_netData.data() + 6, sizeof(uint64_t));
				uint64_t translated = reinterpret_cast<uint64_t>(m_translator.at(reinterpret_cast<void*>(address)));
				memcpy(m_netData.data() + 6, &translated, sizeof(uint64_t));
				LogWriter::WriteLine("Action received");
			}break;
			case 0://CreateObject
			case 5://ChangeGlobalProperty
				break;
			default:
			{
				LogWriter::WriteLine("Net error. Unknown action.");
			}break;
			}
			m_commandHandler.ReadCommandFromStream(stream, m_model);
			if (command == 0)//CreateObject, add to translator
			{
				uint64_t address;
				memcpy(&address, m_netData.data() + 6, sizeof(uint64_t));
				m_translator[reinterpret_cast<void*>(address)] = m_model.Get3DObject(m_model.GetObjectCount() - 1).get();
				LogWriter::WriteLine("CreateObject received");
			}
		}
		else
		{
			LogWriter::WriteLine("Net error. Invalid data received.");
		}
		m_netData.clear();
		m_netTotalSize = 0;
		m_netRecievedSize = 0;
	}
}

bool CNetwork::IsHost() const
{
	return m_host;
}

void CNetwork::SendState()
{
	if (!m_socket)
	{
		LogWriter::WriteLine("Net error. No connection established.");
		return;
	}
	CWriteMemoryStream stream;
	stream.WriteByte(1);//1 For full dump
	stream.WriteSizeT(0);
	m_stateManager.SerializeState(stream, true);
	uint32_t size = static_cast<uint32_t>(stream.GetSize());
	memcpy(&stream.GetData()[1], &size, sizeof(uint32_t));
	m_socket->SendData(stream.GetData(), stream.GetSize());
}

void CNetwork::SendMessage(std::wstring const& message)
{
	if (!m_socket)
	{
		LogWriter::WriteLine("Net error. No connection established.");
		return;
	}
	CWriteMemoryStream data;
	data.WriteByte(0);//0 for text message
	data.WriteWString(message);
	m_socket->SendData(data.GetData(), data.GetSize());
}

void CNetwork::SendAction(ICommand const& command)
{
	if (!m_socket)
	{
		LogWriter::WriteLine("Net error. No connection established.");
		return;
	}
	CWriteMemoryStream result;
	result.WriteByte(2);//2 for action;
	result.WriteSizeT(0);//message size
	command.Serialize(result);
	if (result.GetData()[5] != 5 && result.GetData()[5] != 0)//it is delete, move, rotate or change object property action, so it contains an object addresses that needs to be translated
	{
		uint64_t addr;
		memcpy(&addr, &result.GetData()[6], sizeof(uint64_t));
		auto newAddr = GetAddress(reinterpret_cast<void*>(addr));
		memcpy(&result.GetData()[6], &newAddr, sizeof(uint64_t));
	}
	uint32_t size = static_cast<uint32_t>(result.GetSize());
	memcpy(&result.GetData()[1], &size, sizeof(uint32_t));
	m_socket->SendData(result.GetData(), result.GetSize());
	LogWriter::WriteLine("Action sent.");
}

bool CNetwork::IsConnected()
{
	return m_socket.get() != NULL;
}

void* CNetwork::GetAddress(void* object)
{
	for (auto i = m_translator.begin(); i != m_translator.end(); ++i)
	{
		if (i->second == object)
		{
			return i->first;
		}
	}
	return 0;
}

void CNetwork::AddAddressLocal(std::shared_ptr<IObject> const& obj)
{
	m_translator[obj.get()] = obj.get();
}

void CNetwork::AddAddress(std::shared_ptr<IObject> const& obj, void* address)
{
	m_translator[address] = obj.get();
}

void CNetwork::SetStateRecievedCallback(OnStateRecievedHandler const& onStateRecieved)
{
	m_stateRecievedCallback = onStateRecieved;
}

void CNetwork::SetStringRecievedCallback(OnStringReceivedHandler const& onStringRecieved)
{
	m_stringRecievedCallback = onStringRecieved;
}

void CNetwork::CallStateRecievedCallback()
{
	if (m_stateRecievedCallback) m_stateRecievedCallback();
}