#pragma once
#include <string>
#include <memory>
#include <functional>

class ITask;

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();
	typedef std::function<void()> FunctionHandler;
	typedef std::function<void()> CallbackHandler;
	//Runs function in thread pool. doneCallback will be called in main thread when finished
	void RunFunc(FunctionHandler const& func, CallbackHandler const& callback = CallbackHandler(), unsigned int flags = 0);
	//Queues function to be executed on the main thread
	void QueueCallback(CallbackHandler const& func, unsigned int flags = 0);
	//Runs additional working threads and queued doneCallbacks. Call from main thread as often as possible
	void Update();
	//Returns number of tasks and functions queued
	size_t GetTasksAndFuncsCount();
	//Removes all queued operations and callbacks
	void CancelAll();
	//Task internal functions
	void AddTask(std::shared_ptr<ITask> task);
	void RemoveTask(ITask * task);
	void WaitForTask(ITask & task);
	size_t AddTimedCallback(CallbackHandler const& func, unsigned int time, bool repeat = false, bool executeSkipped = false);
	void RemoveTimedCallback(size_t index);
	enum flags
	{
		//Functions with this flag will be added to the beginning of the queue, not the end
		FLAG_HIGH_PRIORITY = 1,
		//Functions with this flag will be executed in the same thread as the async reading. Speedup on small functions. Has no effect when no async reading is present.
		FLAG_FAST_FUNCTION = 2
	};
	struct Impl;
private:
	std::unique_ptr<Impl> m_pImpl;
};