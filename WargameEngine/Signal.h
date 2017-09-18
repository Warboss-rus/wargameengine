#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace signals
{
class ICallback
{
public:
	virtual ~ICallback() = default;
	virtual void Disconnect() = 0;
};

class SignalConnection
{
public:
	SignalConnection(const std::weak_ptr<ICallback>& callback)
		: m_callback(callback)
	{
	}
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
class Signal
{
public:
	typedef std::function<Result(Arguments...)> Slot;
	SignalConnection Connect(Slot const& handler, int priority = 0, std::string const& tag = "")
	{
		for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
		{
			if (priority <= (*it)->priority)
			{
				auto callback = std::make_shared<Callback>(handler, priority, tag, this);
				m_callbacks.insert(it, callback);
				return SignalConnection(callback);
			}
		}
		auto callback = std::make_shared<Callback>(handler, priority, tag, this);
		m_callbacks.push_back(callback);
		return SignalConnection(callback);
	}
	virtual void operator()(Arguments... args)
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
		m_callbacks.erase(std::remove_if(m_callbacks.begin(), m_callbacks.end(), [&](std::shared_ptr<Callback> const& callback) { return callback->tag == tag; }), m_callbacks.end());
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
		Signal* signal;

		Callback(const Slot& f, int p, const std::string& t, Signal* s)
			: func(f)
			, priority(p)
			, tag(t)
			, signal(s)
		{
		}

		void Disconnect() override
		{
			signal->RemoveByConnection(this);
		}
	};
	using CallbackList = std::vector<std::shared_ptr<Callback>>;

	void RemoveByConnection(const Callback* callback)
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
class ExclusiveSignal : public Signal<bool, Arguments...>
{
public:
	void operator()(Arguments... args) override
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

class ScopedConnection
{
public:
	ScopedConnection(const SignalConnection& connection)
		: m_connection(std::make_shared<CScopedConnectionImpl>(connection))
	{
	}
	ScopedConnection() = default;

private:
	class CScopedConnectionImpl
	{
	public:
		CScopedConnectionImpl(const SignalConnection& connection)
			: m_connection(connection)
		{
		}
		~CScopedConnectionImpl()
		{
			m_connection.Disconnect();
		}

	private:
		SignalConnection m_connection;
	};

	std::shared_ptr<CScopedConnectionImpl> m_connection;
};
}