#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <memory>

class ICallback
{
public:
	virtual ~ICallback() = default;
	virtual void Disconnect() = 0;
};

class CSignalConnection
{
public:
	CSignalConnection(const std::weak_ptr<ICallback>& callback)
		: m_callback(callback)
	{}
	void Disconnect()
	{
		auto callback = m_callback.lock();
		if (callback)
		{
			callback->Disconnect();
		}
	}
private:
	std::weak_ptr<ICallback> m_callback;
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
			if (priority <= (*it)->priority)
			{
				auto callback = std::make_shared<Callback>(handler, priority, tag, this);
				m_callbacks.insert(it, callback);
				return CSignalConnection(callback);
			}
		}
		auto callback = std::make_shared<Callback>(handler, priority, tag, this);
		m_callbacks.push_back(callback);
		return CSignalConnection(callback);
	}
	virtual void operator() (Arguments... args)
	{
		for (auto& callback : m_callbacks)
		{
			if (!callback->func)
			{
				continue;
			}
			callback->func(args...);
		}
	}
	operator bool() const 
	{
		return !m_callbacks.empty();
	}
	void RemoveByTag(std::string const& tag)
	{
		m_callbacks.erase(std::remove_if(m_callbacks.begin(), m_callbacks.end(), [&](std::shared_ptr<Callback> const& callback) {return callback->tag == tag;}), m_callbacks.end());
	}
	void Reset()
	{
		m_callbacks.clear();
	}

protected:
	struct Callback : public ICallback
	{
		Slot func;
		int priority;
		std::string tag;
		CSignal * signal;

		Callback(const Slot& f, int p, const std::string& t, CSignal* s)
			:func(f), priority(p), tag(t), signal(s)
		{}

		void Disconnect() override
		{
			signal->RemoveByConnection(this);
		}
	};
	using CallbackList = std::vector<std::shared_ptr<Callback>>;

	void RemoveByConnection(const Callback * callback)
	{
		auto it = std::find_if(m_callbacks.begin(), m_callbacks.end(), [callback](const std::shared_ptr<Callback>& callbackPtr) {
			return callbackPtr.get() == callback;
		});
		if (it != m_callbacks.end())
		{
			m_callbacks.erase(it);
		}
	}

	CallbackList m_callbacks;
};

template<class... Arguments>
class CExclusiveSignal : public CSignal<bool, Arguments...>
{
public:
	void operator() (Arguments... args) override
	{
		for (auto& callback : this->m_callbacks)
		{
			if (!callback->func)
			{
				continue;
			}
			if (callback->func(args...))
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