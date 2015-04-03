#pragma once
#include <functional>
#include "ThreadPool.h"

template<class T>
class TaskT
{
	typedef std::function<T()> AsyncHandler;
	typedef std::function<void(T)> CallbackHandler;
	typedef std::function<void()> OnFailHandler;
	enum class TaskState
	{
		CREATED,
		QUEUED,
		STARTED,
		COMPLETED,
		READY_FOR_DISPOSE,
		CANCELLED,
		FAILED,
	};
public:
	Task(AsyncHandler const& func, bool start = false)
		:m_handler(func)
		, m_state(CREATED)
	{
		if(start)
		{
			Start();
		}
	}

	void AddOnCompleteHandler(CallbackHandler const& handler)
	{
		m_callback = handler;
	}

	void AddOnFailHandler(CallbackHandler const& handler)
	{
		m_onFail = handler;
	}

	void Cancel()
	{
		m_state = TaskState::CANCELLED;
	}

	void Start()
	{

	}

	const TaskState GetState() const;
	{
		return m_state;
	}
private:
	AsyncHandler m_handler;
	CallbackHandler m_callback;
	OnFailHandler m_onFail;
	TaskState m_state;
};

typedef TaskT<void> Task;