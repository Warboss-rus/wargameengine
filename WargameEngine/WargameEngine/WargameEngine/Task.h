#pragma once
#include <functional>
#include <mutex>
#include "ITask.h"
#include "ThreadPool.h"

template<class T>
class TaskT : public IAsyncTask<T>
{
public:
	TaskT(AsyncHandler const& func, bool start = false)
		: m_handler(func)
		, m_state(TaskState::CREATED)
	{
		
	}

	virtual void AddOnCompleteHandler(CallbackHandler const& handler) override
	{
		std::lock_guard<std::mutex> lk(m_sync);
		if (m_state != TaskState::QUEUED && m_state != TaskState::CREATED)
		{
			throw std::logic_error("Cannot change task when it is already started");
		}
		m_callback = handler;
	}

	virtual void AddOnFailHandler(OnFailHandler const& handler) override
	{
		std::lock_guard<std::mutex> lk(m_sync);
		if (m_state != TaskState::QUEUED && m_state != TaskState::CREATED)
		{
			throw std::logic_error("Cannot change task when it is already started");
		}
		m_onFail = handler;
	}

	virtual void Cancel() override
	{
		std::lock_guard<std::mutex> lk(m_sync);
		m_state = TaskState::CANCELLED;
		ThreadPool::RemoveTask(this);
	}

	virtual void Queue() override
	{
		std::unique_lock<std::mutex> lk(m_sync);
		if (m_state != TaskState::CREATED)
		{
			throw std::runtime_error("Task have already been queued");
		}
		m_state = TaskState::QUEUED;
	}

	virtual const TaskState GetState() const override
	{
		std::lock_guard<std::mutex> lk(m_sync);
		return m_state;
	}
protected:
	TaskT()
		: m_state(TaskState::CREATED)
	{
	}
	void SetTaskState(TaskState state)
	{
		std::lock_guard<std::mutex> lk(m_sync);
		m_state = state;
	}

	AsyncHandler m_handler;
	CallbackHandler m_callback;
	OnFailHandler m_onFail;
	TaskState m_state;
	mutable std::mutex m_sync;
};

template <class T>
class Task : public TaskT<T>
{
public:
	Task(AsyncHandler const& func)
		: TaskT(func)
	{
	}
private:
	virtual void Execute() override
	{
		std::unique_lock<std::mutex> lk(m_sync);
		if (m_state != TaskState::QUEUED)
		{
			throw std::runtime_error("Task is not ready for execution");
		}
		m_state = TaskState::STARTED;
		lk.unlock();
		try
		{
			T result = m_handler();
			SetTaskState(TaskState::COMPLETED);
			if (m_callback)
			{
				ThreadPool::QueueCallback([this, result] {m_callback(result); });
			}
		}
		catch (std::exception const& e)
		{
			SetTaskState(TaskState::FAILED);
			if (m_onFail)
			{
				ThreadPool::QueueCallback([this]() {m_onFail(e);});
			}
		}
	}
};