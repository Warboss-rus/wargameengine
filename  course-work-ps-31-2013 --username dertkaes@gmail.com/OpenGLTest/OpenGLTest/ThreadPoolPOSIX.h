#include <string>
#include <list>

class CThreadPoolPOSIX
{
public:
	CThreadPoolPOSIX();
	~CThreadPoolPOSIX();
	//Runs function in separate thread.
	static void RunFunc(void* (_cdecl* func)(void *), void* param);
	//Runs function in separate thread. doneCallback will be called in main thread when finished
	static void RunFunc(void (_cdecl* func)(void *), void* param, void(*doneCallback)());
	//Runs function in separate thread. doneCallback will be called in main thread when finished with initial function result as the parameter
	static void RunFunc(void* (_cdecl* func)(void *), void* param, void(*doneCallback)(void*));
	//Reads file content and runs a function with a data, size and param as parameters. doneCallback will be called in main thread when finished
	static void AsyncReadFile(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)());
	//Reads file content and runs a function with a data, size and param as parameters. doneCallback will be called in main thread when finished with initial function result as the parameter
	static void AsyncReadFile(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*));
	//Runs queued doneCallbacks. Call from main thread as often as possible
	static void Update();
private:
	static void QueueCallback(void(*callback)());
	static void QueueCallback(void(*callback)(void*), void *params);
	static std::list<void(*)()> m_callbacks;
	static std::list<std::pair<void(*)(void*), void*>> m_callbacks2;
	friend void* Func(void* param);
	friend void* Func2(void* param);
	friend void* ReadData(void* param);
	friend void* ReadData2(void* param);
};