#include <string>

class CThreadPool
{
public:
	CThreadPool() {}
	//Runs function in separate thread.
	static void RunFunc(void (* func)(void*), void* param);
	//Runs function in separate thread. doneCallback will be called in main thread when finished
	static void RunFunc(void (* func)(void*), void* param, void (* doneCallback)());
	//Runs function in separate thread. doneCallback will be called in main thread when finished with initial function result as the parameter
	static void RunFunc(void* (* func)(void*), void* param, void (* doneCallback)(void*));
	//Reads file content and runs a function with a data, size and param as parameters. doneCallback will be called in main thread when finished
	static void AsyncReadFile(std::string const& path, void (* func)(void*, unsigned int, void*), void* param, void (* doneCallback)());
	//Reads file content and runs a function with a data, size and param as parameters. doneCallback will be called in main thread when finished with initial function result as the parameter
	static void AsyncReadFile(std::string const& path, void* (* func)(void*, unsigned int, void*), void* param, void (* doneCallback)(void*));
	//Runs queued doneCallbacks. Call from main thread as often as possible
	static void Update() {}
};