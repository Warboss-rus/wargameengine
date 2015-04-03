#include <string>
#include <list>
#include <memory>

class ThreadPool
{
public:
	//Runs function in thread pool.
	static void RunFunc(void* (*func)(void*), void* param, unsigned int flags = 0);
	//Runs function in thread pool. doneCallback will be called in main thread when finished
	static void RunFunc(void(*func)(void*), void* param, void(*doneCallback)(), unsigned int flags = 0);
	//Runs function in thread pool. doneCallback will be called in main thread when finished with initial function result as the parameter
	static void RunFunc(void* (*func)(void*), void* param, void(*doneCallback)(void*), unsigned int flags = 0);
	//Reads file content and runs a function with a data, size and param as parameters. doneCallback will be called in main thread when finished
	static void AsyncReadFile(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(), unsigned int flags = 0);
	//Reads file content and runs a function with a data, size and param as parameters. doneCallback will be called in main thread when finished with initial function result as the parameter
	static void AsyncReadFile(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*), unsigned int flags = 0);
	//Runs additional working threads and queued doneCallbacks. Call from main thread as often as possible
	static void Update();
	//Waits until all active jobs complete. Main thread is also used as a working thread.
	static void WaitAll();
	//Returns time before thread closed if it has no work to do.
	static int GetWorkerTimeout();
	//Sets the time before thread closed if it has no work to do.
	static void SetWorkerTimeout(int timeout);
	//Removes all queued operations and callbacks
	static void CancelAll();
	enum flags
	{
		//Functions with this flag will be added to the beginning of the queue, not the end
		FLAG_HIGH_PRIORITY = 1,
		//Functions with this flag will be executed in the same thread as the async reading. Speedup on small functions. Has no effect when no async reading is present.
		FLAG_FAST_FUNCTION = 2
	};
	struct Impl;
private:
	static std::unique_ptr<Impl> m_pImpl;
	static void* ReadData(void* param);
	static void* ReadData2(void* param);
	static void* WorkerThread(void* param);
	static void ProcessData(void* param);
	static void* ProcessData2(void* param);
};