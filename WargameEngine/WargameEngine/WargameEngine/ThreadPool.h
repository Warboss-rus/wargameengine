#include <string>
#include <list>

struct sRunFunc;
struct sRunFunc2;

#include "Threading.h"

class ThreadPool
{
public:
	//Runs function in threadpool.
	static void RunFunc(void* (*func)(void*), void* param, unsigned int flags = 0);
	//Runs function in threadpool. doneCallback will be called in main thread when finished
	static void RunFunc(void(*func)(void*), void* param, void(*doneCallback)(), unsigned int flags = 0);
	//Runs function in threadpool. doneCallback will be called in main thread when finished with initial function result as the parameter
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
	enum flags
	{
		//Functions with this flag will be added to the beginning of the queue, not the end
		FLAG_HIGH_PRIORITY = 1,
		//Functions with this flag will be executed in the same thread as the async reading. Speedup on small functions. Has no effect when no async reading is present.
		FLAG_FAST_FUNCTION = 2
	};
	
private:
	static void QueueCallback(void(*callback)());
	static void QueueCallback(void(*callback)(void*), void *params);
	static void QueueFunc(sRunFunc * func, unsigned int flags = 0);
	static void QueueFunc(sRunFunc2 * func, unsigned int flags = 0);
	static void ReportThreadClose();
	static void* Func(void* param);
	static void* Func2(void* param);
	static void* ReadData(void* param);
	static void* ReadData2(void* param);
	static void ProcessData(void* param);
	static void* ProcessData2(void* param);
	static void* WorkerThread(void* param);
	static std::list<void(*)()> m_callbacks;
	static std::list<std::pair<void(*)(void*), void*>> m_callbacks2;
	static std::list<sRunFunc*> m_funcs;
	static std::list<sRunFunc2*> m_funcs2;
	static int m_currentThreads;
	static int m_maxThreads;
	static int m_threadsTimeout;
};