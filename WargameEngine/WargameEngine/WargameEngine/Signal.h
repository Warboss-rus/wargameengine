#pragma once
#include <vector>
#include <functional>
#include <algorithm>

template<typename... Arguments>
class CSignal
{
public:
	typedef std::function<bool(Arguments...)> Func;
	void Connect(Func const& handler, int priority = 0, std::string const& tag = "")
	{
		for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
		{
			if (priority <= it->priority)
			{
				m_callbacks.insert(it, sCallback({ handler, priority, tag }));
				return;
			}
		}
		m_callbacks.push_back({ handler, priority, tag });
	}
	void operator() (Arguments... args)
	{
		for (auto callback : m_callbacks)
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
	void RemoveByTag(std::string const& tag)
	{
		if (m_callbacks.empty()) return;
		std::remove_if(m_callbacks.begin(), m_callbacks.end(), [&](sCallback const& callback) {return callback.tag == tag;});
	}
private:
	struct sCallback
	{
		Func func;
		int priority;
		std::string tag;
	};
	std::vector<sCallback> m_callbacks;
};

template<class... Args>
class CScopedConnection
{
public:
	CScopedConnection(int id, CSignal<Args...> & signal)
		:m_signal(signal), m_id(id)
	{}
	~CScopedConnection() { m_signal.Remove(m_id); }
private:
	int m_id;
	CSignal<Args...> & m_signal;
};