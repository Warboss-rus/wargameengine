#include <string>
#include <list>

struct sRunFunc;
struct sRunFunc2;

class ThreadPool
{
public:
	//Runs function in separate thread.
	static void RunFunc(void* (*func)(void*), void* param);
	//Runs function in separate thread. doneCallback will be called in main thread when finished
	static void RunFunc(void(*func)(void*), void* param, void(*doneCallback)());
	//Runs function in separate thread. doneCallback will be called in main thread when finished with initial function result as the parameter
	static void RunFunc(void* (*func)(void*), void* param, void(*doneCallback)(void*));
	//Reads file content and runs a function with a data, size and param as parameters. doneCallback will be called in main thread when finished
	static void AsyncReadFile(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)());
	//Reads file content and runs a function with a data, size and param as parameters. doneCallback will be called in main thread when finished with initial function result as the parameter
	static void AsyncReadFile(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*));
	//Runs queued doneCallbacks. Call from main thread as often as possible
	static void Update();
private:
	static void QueueCallback(void(*callback)());
	static void QueueCallback(void(*callback)(void*), void *params);
	static void QueueFunc(sRunFunc * func);
	static void QueueFunc(sRunFunc2 * func);
	static void* Func(void* param);
	static void* Func2(void* param);
	static void* ReadData(void* param);
	static void* ReadData2(void* param);
	static std::list<void(*)()> m_callbacks;
	static std::list<std::pair<void(*)(void*), void*>> m_callbacks2;
	static std::list<sRunFunc*> m_funcs;
	static std::list<sRunFunc2*> m_funcs2;
	static int m_currentThreads;
	static int m_maxThreads;
};