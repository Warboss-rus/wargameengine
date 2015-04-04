#pragma once
#include <functional>
#include <mutex>
#include "ITask.h"
#include "ThreadPool.h"

template<class T>
class TaskT : public ITask
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
		lock_guard<std::mutex> lk(m_sync);
		if (m_state != TaskState::QUEUED || m_state != TaskState::CREATED)
		{
			throw std::logic_error("Cannot change task when it is already started");
		}
		m_callback = handler;
	}

	void AddOnFailHandler(CallbackHandler const& handler)
	{
		lock_guard<std::mutex> lk(m_sync);
		if (m_state != TaskState::QUEUED || m_state != TaskState::CREATED)
		{
			throw std::logic_error("Cannot change task when it is already started");
		}
		m_onFail = handler;
	}

	void Cancel()
	{
		std::lock_guard<std::mutex> lk(m_sync);
		m_state = TaskState::CANCELLED;
	}

	void Start()
	{
		std::unique_lock<std::mutex> lk(m_sync);
		if (m_state != TaskState::CREATED)
		{
			throw std::runtime_error("Task have already been queued");
		}
		ThreadPool::AddTask(this);
	}

	const TaskState GetState() const;
	{
		std::lock_guard<std::mutex> lk(m_sync);
		return m_state;
	}
private:
	virtual void Execute() override
	{
		std::unique_lock<std::mutex> lk(m_sync);
		if(m_state != TaskState::QUEUED)
		{
			throw std::runtime_error("Task is not ready for execution");
		}
		m_state = TaskState::STARTED;
		lk.unlock();
		try
		{
			T result = m_handler();
			lk.lock();
			m_state = TaskState::COMPLETED;
			if (m_onFail)
			{
				ThreadPool::AddTaskCallback([this, result] {m_callback(result); });
			}
		}
		catch (std::exception const& e)
		{
			lk.lock();
			m_state = TaskState::FAILED;
			if(m_onFail)
			{
				ThreadPool::AddTaskCallback(m_onFail);
			}
		}
	}

	AsyncHandler m_handler;
	CallbackHandler m_callback;
	OnFailHandler m_onFail;
	TaskState m_state;
	std::mutex m_sync;
};

typedef TaskT<void> Task;