#pragma once
#include <thread>

class ThreadPool;

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