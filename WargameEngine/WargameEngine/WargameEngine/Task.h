#pragma once
#include "ITask.h"
#include "ThreadPool.h"
#include <functional>
#include <mutex>

namespace wargameEngine
{
class TaskBase : public ITask
{
public:
	typedef std::function<void()> AsyncHandler;
	typedef std::function<void()> CallbackHandler;
	typedef std::function<void(std::exception const&)> OnFailHandler;

	TaskBase(AsyncHandler const& func, ThreadPool& threadPool)
		: m_handler(func)
		, m_state(TaskState::CREATED)
		, m_threadPool(threadPool)
	{
	}

	void AddOnCompleteHandler(CallbackHandler const& handler)
	{
		std::lock_guard<std::mutex> lk(m_sync);
		if (m_state != TaskState::QUEUED && m_state != TaskState::CREATED)
		{
			throw std::logic_error("Cannot change task when it is already started");
		}
		m_callback = handler;
	}

	void AddOnFailHandler(OnFailHandler const& handler)
	{
		std::lock_guard<std::mutex> lk(m_sync);
		if (m_state != TaskState::QUEUED && m_state != TaskState::CREATED)
		{
			throw std::logic_error("Cannot change task when it is already started");
		}
		m_onFail = handler;
	}

	void Cancel()
	{
		std::lock_guard<std::mutex> lk(m_sync);
		m_state = TaskState::CANCELLED;
		m_threadPool.RemoveTask(this);
	}

	void Queue() override
	{
		std::unique_lock<std::mutex> lk(m_sync);
		if (m_state != TaskState::CREATED)
		{
			throw std::runtime_error("Task have already been queued");
		}
		m_state = TaskState::QUEUED;
	}

	const TaskState GetState() const override
	{
		std::lock_guard<std::mutex> lk(m_sync);
		return m_state;
	}

protected:
	TaskBase(ThreadPool& threadPool)
		: m_state(TaskState::CREATED)
		, m_threadPool(threadPool)
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
	ThreadPool& m_threadPool;
	mutable std::mutex m_sync;
};

class Task : public TaskBase
{
public:
	Task(AsyncHandler const& func, ThreadPool& threadPool)
		: TaskBase(func, threadPool)
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
			m_handler();
			SetTaskState(TaskState::COMPLETED);
			if (m_callback)
			{
				m_threadPool.QueueCallback([this] {
					m_callback();
					m_threadPool.RemoveTask(this);
				});
			}
		}
		catch (std::exception const& e)
		{
			SetTaskState(TaskState::FAILED);
			if (m_onFail)
			{
				m_threadPool.QueueCallback([this, &e]() { m_onFail(e); });
			}
		}
	}
};
}