#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <memory>

class CSignalConnection
{
public:
	CSignalConnection(std::function<void()> const& onDisconnect = std::function<void()>())
		: m_onDisconnect(onDisconnect)
	{}
	void Disconnect()
	{
		if (m_onDisconnect) m_onDisconnect();
	}
private:
	std::function<void()> m_onDisconnect;
};

template<typename Result, typename... Arguments>
class CSignal
{
public:
	typedef std::function<Result(Arguments...)> Slot;
	CSignalConnection Connect(Slot const& handler, int priority = 0, std::string const& tag = "")
	{
		for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
		{
			if (priority <= it->priority)
			{
				return CSignalConnection(std::bind(&CSignal::RemoveByConnection, this, m_callbacks.insert(it, sCallback({ handler, priority, tag }))));
			}
		}
		return CSignalConnection(std::bind(&CSignal::RemoveByConnection, this, m_callbacks.insert(m_callbacks.end(), { handler, priority, tag })));
	}
	virtual void operator() (Arguments... args)
	{
		for (auto callback : m_callbacks)
		{
			if (!callback.func)
			{
				continue;
			}
			callback.func(args...);
		}
	}
	operator bool() const 
	{
		return !m_callbacks.empty();
	}
	void RemoveByTag(std::string const& tag)
	{
		m_callbacks.erase(std::remove_if(m_callbacks.begin(), m_callbacks.end(), [&](sCallback const& callback) {return callback.tag == tag;}), m_callbacks.end());
	}
	void Reset()
	{
		m_callbacks.clear();
	}
private:
	struct sCallback
	{
		Slot func;
		int priority;
		std::string tag;
	};
	void RemoveByConnection(typename std::vector<sCallback>::iterator const& it)
	{
		if (it > m_callbacks.begin() && it < m_callbacks.end())
		{
			m_callbacks.erase(it);
		}
	}
protected:
	std::vector<sCallback> m_callbacks;
};

template<class... Arguments>
class CExclusiveSignal : public CSignal<bool, Arguments...>
{
public:
	void operator() (Arguments... args) override
	{
		for (auto& callback : this->m_callbacks)
		{
			if (!callback.func)
			{
				continue;
			}
			if (callback.func(args...))
			{
				return;
			}
		}
	}
};

class CScopedConnection
{
public:
	CScopedConnection(const CSignalConnection & connection)
		: m_connection(std::make_shared<CScopedConnectionImpl>(connection))
	{}
	CScopedConnection() = default;
private:
	class CScopedConnectionImpl
	{
	public:
		CScopedConnectionImpl(const CSignalConnection & connection)
			:m_connection(connection)
		{
		}
		~CScopedConnectionImpl()
		{
			m_connection.Disconnect();
		}
	private:
		CSignalConnection m_connection;
	};

	std::shared_ptr<CScopedConnectionImpl> m_connection;
};