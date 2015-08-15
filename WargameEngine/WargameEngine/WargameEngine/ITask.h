#pragma once
#include <thread>

class ThreadPool;
struct ThreadPool::Impl;

class ITask
{
public:
	virtual ~ITask() {}

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

	virtual const TaskState GetState() const = 0;
private:
	virtual void Execute() = 0;
	virtual void Queue() = 0;

	friend struct ThreadPool::Impl;
	friend void WaitForTask(ITask & task);
};

template <class T>
class IAsyncTask : public ITask
{
public:
	
	typedef std::function<T()> AsyncHandler;
	typedef std::function<void(T)> CallbackHandler;
	typedef std::function<void(std::exception const&)> OnFailHandler;

	virtual void AddOnCompleteHandler(CallbackHandler const& handler) = 0;
	virtual void AddOnFailHandler(OnFailHandler const& handler) = 0;
	virtual void Cancel() = 0;
};

inline void WaitForTask(ITask & task)
{
	for (;;)
	{
		switch (task.GetState())
		{
		case ITask::TaskState::QUEUED:
		case ITask::TaskState::STARTED:
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			ThreadPool::Update();
		} break;
		case ITask::TaskState::COMPLETED:
		case ITask::TaskState::READY_FOR_DISPOSE:
		case ITask::TaskState::CANCELLED:
		case ITask::TaskState::FAILED:
			return;
		default:
			throw std::exception("Invalid task state");
		}
	}
}