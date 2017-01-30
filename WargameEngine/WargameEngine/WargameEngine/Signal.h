#pragma once
#include <list>
#include <functional>
#include <algorithm>

template<typename Result, typename... Arguments>
class CSignal;

template<typename Result, typename... Arguments>
class CSignalConnection
{
	typedef CSignal<Result, Arguments...> Signal;
	typedef typename std::list<typename Signal::sCallback>::iterator Iterator;
public:
	CSignalConnection(Iterator const& it, Signal & signal)
		:m_it(it), m_signal(signal)
	{}
	CSignalConnection(CSignalConnection const& other) = default;
	void Disconnect()
	{
		m_signal.RemoveByConnection(m_it);
	}
private:
	Iterator m_it;
	Signal& m_signal;
};

template<typename Result, typename... Arguments>
class CSignal
{
public:
	typedef std::function<Result(Arguments...)> Func;
	typedef CSignalConnection<Result, Arguments...> Connection;
	Connection Connect(Func const& handler, int priority = 0, std::string const& tag = "")
	{
		for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
		{
			if (priority <= it->priority)
			{
				return Connection(m_callbacks.insert(it, sCallback({ handler, priority, tag })), *this);
			}
		}
		return Connection(m_callbacks.insert(m_callbacks.end(), { handler, priority, tag }), *this);
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
		if (m_callbacks.empty()) return;
		m_callbacks.erase(std::remove_if(m_callbacks.begin(), m_callbacks.end(), [&](sCallback const& callback) {return callback.tag == tag;}), m_callbacks.end());
	}
private:
	struct sCallback
	{
		Func func;
		int priority;
		std::string tag;
	};
	void RemoveByConnection(typename std::list<sCallback>::iterator const& it)
	{
		m_callbacks.erase(it);
	}
	friend class CSignalConnection<Result, Arguments...>;
protected:
	std::list<sCallback> m_callbacks;
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

template<typename Result, typename... Arguments>
class CScopedConnection : public CSignalConnection<Result, Arguments...>
{
public:
	CScopedConnection(CSignalConnection<Result, Arguments...> const& connection)
		: CSignalConnection<Result, Arguments...>(connection)
	{}
	~CScopedConnection() { this->Disconnect(); }
};