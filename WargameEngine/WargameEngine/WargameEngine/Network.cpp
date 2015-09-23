#include "Network.h"
#include <string.h>
#include "LogWriter.h"
#include "model/GameModel.h"
#include "model/Object.h"
#include "controller/CommandHandler.h"
#include "controller/IStateManager.h"

CNetwork::CNetwork(IStateManager & stateManager, CCommandHandler & commandHandler, IGameModel & model)
	:m_host(true)
	, m_netData(NULL)
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
	m_socket.reset(new CNetSocket(port));
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
	m_socket.reset(new CNetSocket(ip, port));
	m_host = false;
}

void CNetwork::Stop()
{
	m_socket.reset();
	m_children.clear();
	m_host = true;
	m_netData = NULL;
	m_netRecievedSize = 0;
	m_netTotalSize = 0;
}

void CNetwork::Update()
{
	if (!m_socket) return;
	int result;
	if (m_netData)
	{
		result = m_socket->RecieveData(m_netData + m_netRecievedSize, m_netTotalSize - m_netRecievedSize);
		m_netRecievedSize += result;
	}
	else
	{
		char data[128];
		result = m_socket->RecieveData(data, 128);
		if (result > 0)
		{
			m_netTotalSize = *(unsigned int*)&data[1];
			m_netData = new char[m_netTotalSize];
			m_netRecievedSize = result;
			memcpy(m_netData, data, result);
		}
	}
	if (result == 0)
	{
		m_socket.reset();
	}
	if (m_netData && m_netRecievedSize >= m_netTotalSize && m_netRecievedSize > 4)
	{
		if (m_netData[0] == 0) //string
		{
			if (m_stringRecievedCallback)
				m_stringRecievedCallback(m_netData + 5);
			char* str = new char[12 + m_netTotalSize];
			sprintf(str, "String recieved:%s", (const char*)(m_netData + 5));
			LogWriter::WriteLine(str);
			delete[] str;
		}
		else if (m_netData[0] == 1) //state
		{
			char state[30];
			sprintf(state, "State Recieved. Size=%d.", m_netRecievedSize);
			LogWriter::WriteLine(state);
			m_stateManager.SetState(m_netData + 5, true);
			if (m_stateRecievedCallback) m_stateRecievedCallback();
		}
		else if (m_netData[0] == 2) //command
		{
			char * data = m_netData + 4;
			char type = data[1];
			switch (type)
			{
			case 0://CreateObject
			{
				double pos[3];
				unsigned int address, size;
				memcpy(&address, data + 2, 4);
				memcpy(&pos, data + 6, 24);
				memcpy(&size, data + 30, 4);
				std::string path;
				path.resize(size);
				memcpy(&path[0], data + 34, size);
				std::shared_ptr<IObject> obj = std::shared_ptr<IObject>(new CObject(path, pos[0], pos[1], pos[2], true));
				m_commandHandler.AddNewCreateObject(obj, m_model, false);
				m_translator[address] = obj;
				LogWriter::WriteLine("CreateObject received");
			}break;
			case 1://DeleteObject
			{
				unsigned int address;
				memcpy(&address, data + 2, 4);
				m_commandHandler.AddNewDeleteObject(GetObject(address), m_model, false);
				m_translator.erase(address);
				LogWriter::WriteLine("DeleteObject received");
			}break;
			case 2://MoveObject
			{
				unsigned int address;
				double x, y;
				memcpy(&address, data + 2, 4);
				memcpy(&x, data + 6, 8);
				memcpy(&y, data + 14, 8);
				m_commandHandler.AddNewMoveObject(GetObject(address), x, y, false);
				LogWriter::WriteLine("MoveObject received");
			}break;
			case 3://RotateObject
			{
				unsigned int address;
				double rot;
				memcpy(&address, data + 2, 4);
				memcpy(&rot, data + 6, 8);
				m_commandHandler.AddNewRotateObject(GetObject(address), rot, false);
				LogWriter::WriteLine("RotateObject received");
			}break;
			case 4://ChangeProperty
			{
				unsigned int address, size;
				std::string key, newvalue, oldvalue;
				memcpy(&address, data + 2, 4);
				memcpy(&size, data + 6, 4);
				key.resize(size);
				memcpy(&key[0], data + 10, size);
				unsigned int begin = size + 10;
				memcpy(&size, data + begin, 4);
				newvalue.resize(size);
				memcpy(&newvalue[0], data + begin + 4, size);
				begin += size + 4;
				memcpy(&size, data + begin, 4);
				oldvalue.resize(size);
				memcpy(&oldvalue[0], data + begin + 4, size);
				m_commandHandler.AddNewChangeProperty(GetObject(address), key, newvalue, false);
				LogWriter::WriteLine("ChangeProperty received");
			}break;
			case 5://ChangeGlobalProperty
			{
				unsigned int size;
				std::string key, newvalue, oldvalue;
				memcpy(&size, data + 2, 4);
				key.resize(size);
				memcpy(&key[0], data + 6, size);
				unsigned int begin = size + 6;
				memcpy(&size, data + begin, 4);
				newvalue.resize(size);
				memcpy(&newvalue[0], data + begin + 4, size);
				begin += size + 4;
				memcpy(&size, data + begin, 4);
				oldvalue.resize(size);
				memcpy(&oldvalue[0], data + begin + 4, size);
				m_commandHandler.AddNewChangeGlobalProperty(key, newvalue, m_model, false);
				LogWriter::WriteLine("CreateGlobalProperty received");
			}break;
			case 6://PlayAnimation
			{
				unsigned int address, size;
				char loop;
				float speed;
				std::string anim;
				memcpy(&address, data + 2, 4);
				loop = data[6];
				memcpy(&speed, data + 7, sizeof(float));
				memcpy(&size, data + 11, sizeof(unsigned int));
				anim.resize(size);
				memcpy(&anim[0], data + 15, size);
				m_commandHandler.AddNewPlayAnimation(GetObject(address), anim, loop, speed, false);
				LogWriter::WriteLine("PlayAnimation received");
			}break;
			case 7://GoTo
			{
				unsigned int address, size;
				double x, y, speed;
				float animSpeed;
				std::string anim;
				memcpy(&address, data + 2, 4);
				memcpy(&x, data + 6, sizeof(double));
				memcpy(&y, data + 14, sizeof(double));
				memcpy(&speed, data + 22, sizeof(double));
				memcpy(&animSpeed, data + 30, sizeof(float));
				memcpy(&size, data + 34, sizeof(unsigned int));
				anim.resize(size);
				memcpy(&anim[0], data + 38, size);
				m_commandHandler.AddNewGoTo(GetObject(address), x, y, speed, anim, animSpeed, false);
				LogWriter::WriteLine("PlayAnimation received");
			}break;
			default:
			{
				LogWriter::WriteLine("Net error. Unknown action.");
			}break;
			}
			
		}
		else
		{
			LogWriter::WriteLine("Net error. Invalid data received.");
		}
		delete[] m_netData;
		m_netData = NULL;
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
	std::vector<char> result = m_stateManager.GetState(true);
	m_socket->SendData(&result[0], result.size());//1 For full dump
}

void CNetwork::SendMessage(std::string const& message)
{
	if (!m_socket)
	{
		LogWriter::WriteLine("Net error. No connection established.");
		return;
	}
	char * data = new char[message.size() + 6];
	data[0] = 0;//0 for text message
	*(unsigned int*)&data[1] = message.size() + 6;
	memcpy(&data[5], message.c_str(), message.size() + 1);
	m_socket->SendData(data, message.size() + 6);
	delete[] data;
}

void CNetwork::SendAction(std::vector<char> const& command, bool execute)
{
	if (!m_socket)
	{
		LogWriter::WriteLine("Net error. No connection established.");
		return;
	}
	std::vector<char> result;
	result.resize(5);
	result[0] = 2;//2 for action;
	std::vector<char> msg = command;
	if (msg[0] < 5 && msg[0] > 0)//it is delete, move, rotate or change object property action, so it contains an object adresses that needs to be translated
	{
		unsigned int addr;
		memcpy(&addr, &msg[1], 4);
		unsigned int newAddr = GetAddress((IObject*)addr);
		memcpy(&msg[1], &newAddr, 4);
	}
	if (!execute) msg[0] = -msg[0];
	unsigned int size = msg.size() + 5;
	memcpy(&result[1], &size, 4);
	result.insert(result.end(), msg.begin(), msg.end());
	m_socket->SendData(&result[0], result.size());
	LogWriter::WriteLine("Action sent.");
}

bool CNetwork::IsConnected()
{
	return m_socket.get() != NULL;
}

unsigned int CNetwork::GetAddress(std::shared_ptr<IObject> object)
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

unsigned int CNetwork::GetAddress(IObject* object)
{
	for (auto i = m_translator.begin(); i != m_translator.end(); ++i)
	{
		if (i->second.get() == object)
		{
			return i->first;
		}
	}
	return 0;
}

std::shared_ptr<IObject> CNetwork::GetObject(unsigned int address)
{
	return m_translator[address];
}

void CNetwork::AddAddressLocal(std::shared_ptr<IObject> obj)
{
	m_translator[(uintptr_t)obj.get()] = obj;
}

void CNetwork::AddAddress(std::shared_ptr<IObject> obj, unsigned int address)
{
	m_translator[address] = obj;
}

void CNetwork::SetStateRecievedCallback(std::function<void()> onStateRecieved)
{
	m_stateRecievedCallback = onStateRecieved;
}

void CNetwork::SetStringRecievedCallback(std::function<void(const char*)> onStringRecieved)
{
	m_stringRecievedCallback = onStringRecieved;
}

void CNetwork::CallStateRecievedCallback()
{
	if (m_stateRecievedCallback) m_stateRecievedCallback();
}