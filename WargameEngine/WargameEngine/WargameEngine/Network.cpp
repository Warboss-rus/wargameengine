#include "Network.h"
#include "LogWriter.h"
#include "model\GameModel.h"
#include "model\3dObject.h"

std::shared_ptr<CNetwork> CNetwork::m_instance;

std::weak_ptr<CNetwork> CNetwork::GetInstance()
{
	if (!m_instance)
	{
		m_instance.reset(new CNetwork());
	}
	return std::weak_ptr<CNetwork>(m_instance);
}

void CNetwork::FreeInstance()
{
	m_instance.reset();
}

void CNetwork::Host(unsigned int port)
{
	if (m_socket)
	{
		CLogWriter::WriteLine("Net error. Already connected");
		return;
	}
	m_socket.reset(new CNetSocket(port));
	m_host = true;
}

void CNetwork::Client(const char * ip, unsigned short port)
{
	if (m_socket)
	{
		CLogWriter::WriteLine("Net error. Already connected");
		return;
	}
	m_socket.reset(new CNetSocket(ip, port));
	m_host = false;
}

void CNetwork::Stop()
{
	m_socket.reset();
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
	if (m_netRecievedSize >= m_netTotalSize && m_netRecievedSize > 4)
	{
		if (m_netData[0] == 0) //string
		{
			/*if (m_stringRecievedCallback)
				m_stringRecievedCallback(m_netData + 5);*/
			char* str = new char[12 + m_netTotalSize];
			sprintf(str, "String recieved:%s", (const char*)(m_netData + 5));
			CLogWriter::WriteLine(str);
			delete[] str;
		}
		else if (m_netData[0] == 1) //state
		{
			char state[30];
			sprintf(state, "State Recieved. Size=%d.", m_netRecievedSize);
			CLogWriter::WriteLine(state);
			CGameModel::GetInstance().lock()->SetState(m_netData + 5);
			//if (m_stateRecievedCallback) m_stateRecievedCallback();
		}
		else if (m_netData[0] == 2) //command
		{
			char * data = m_netData + 4;
			bool action = data[1] < 0;
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
				std::shared_ptr<IObject> obj = std::shared_ptr<IObject>(new C3DObject(path, pos[0], pos[1], pos[2], true));
				CCommandHandler::GetInstance().lock()->AddNewCreateObject(obj, false);
				if(!IsHost()) m_translator[address] = obj;
				CLogWriter::WriteLine("CreateObject received");
			}break;
			case 1://DeleteObject
			{
				unsigned int address;
				memcpy(&address, data + 2, 4);
				CCommandHandler::GetInstance().lock()->AddNewDeleteObject(GetObject(address), false);
				if(!IsHost()) m_translator.erase(address);
				CLogWriter::WriteLine("DeleteObject received");
			}break;
			case 2://MoveObject
			{
				unsigned int address;
				double x, y;
				memcpy(&address, data + 2, 4);
				memcpy(&x, data + 6, 8);
				memcpy(&y, data + 14, 8);
				CCommandHandler::GetInstance().lock()->AddNewMoveObject(GetObject(address), x, y, false);
				CLogWriter::WriteLine("MoveObject received");
			}break;
			case 3://RotateObject
			{
				unsigned int address;
				double rot;
				memcpy(&address, data + 2, 4);
				memcpy(&rot, data + 6, 8);
				CCommandHandler::GetInstance().lock()->AddNewRotateObject(GetObject(address), rot, false);
				CLogWriter::WriteLine("RotateObject received");
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
				CCommandHandler::GetInstance().lock()->AddNewChangeProperty(GetObject(address), key, newvalue, false);
				CLogWriter::WriteLine("ChangeProperty received");
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
				CCommandHandler::GetInstance().lock()->AddNewChangeGlobalProperty(key, newvalue, false);
				CLogWriter::WriteLine("CreateGlobalProperty received");
			}break;
			default:
			{
				CLogWriter::WriteLine("Net error. Unknown action.");
			}break;
			}
			
		}
		else
		{
			CLogWriter::WriteLine("Net error. Invalid data received.");
		}
		delete[] m_netData;
		m_netData = NULL;
		m_netTotalSize = 0;
		m_netRecievedSize = 0;
	}
}

void CNetwork::SendState()
{
	if (!m_socket)
	{
		CLogWriter::WriteLine("Net error. No connection established.");
		return;
	}
	std::vector<char> result = CGameModel::GetInstance().lock()->GetState();
	m_socket->SendData(&result[0], result.size());//1 For full dump
}

void CNetwork::SendMessag(std::string const& message)
{
	if (!m_socket)
	{
		CLogWriter::WriteLine("Net error. No connection established.");
		return;
	}
	char * data = new char[message.size() + 6];
	data[0] = 0;//0 for text message
	*(unsigned int*)&data[1] = message.size() + 6;
	memcpy(&data[5], message.c_str(), message.size() + 1);
	m_socket->SendData(data, message.size() + 6);
	delete[] data;
}

void CNetwork::SendAction(ICommand* command, bool execute)
{
	if (!m_socket)
	{
		CLogWriter::WriteLine("Net error. No connection established.");
		return;
	}
	std::vector<char> result;
	result.resize(5);
	result[0] = 2;//2 for action;
	std::vector<char> msg = command->Serialize();
	if (!execute) msg[0] = -msg[0];
	unsigned int size = msg.size() + 5;
	memcpy(&result[1], &size, 4);
	result.insert(result.end(), msg.begin(), msg.end());
	m_socket->SendData(&result[0], result.size());
	CLogWriter::WriteLine("Action sent.");
}

unsigned int CNetwork::GetAddress(std::shared_ptr<IObject> object)
{
	if (IsHost()) return (unsigned int)object.get();
	for (auto i = m_translator.begin(); i != m_translator.end(); ++i)
	{
		if (i->second == object)
		{
			return i->first;
		}
	}
	return 0;
}

std::shared_ptr<IObject> CNetwork::GetObject(unsigned int address)
{
	if (IsHost()) return CGameModel::GetInstance().lock()->Get3DObject((IObject*)address);
	return m_translator[address];
}