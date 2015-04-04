#include "ThreadPool.h"
#include "LogWriter.h"
#include <mutex>
#include <future>
#include <deque>

void ProcessData(void* param);
void* ProcessData2(void* param);

struct ThreadPool::Impl
{
	struct sRunFunc
	{
		sRunFunc(void(*f)(void*), void* p, void(*c)(), unsigned int fl) :func(f), param(p), callback(c), flags(fl) {}
		void(*func)(void*);
		void* param;
		void(*callback)();
		unsigned int flags;
	};

	struct sRunFunc2
	{
		sRunFunc2(void* (*f)(void*), void* p, void(*c)(void*), unsigned int fl) :func(f), param(p), callback(c), flags(fl) {}
		void* (*func)(void*);
		void* param;
		void(*callback)(void*);
		unsigned int flags;
	};

	struct sAsyncRead
	{
		sAsyncRead(std::string const& p, void(*f)(void*, unsigned int, void*), void* par, void(*c)(), unsigned int fl) :path(p), func(f), param(par), callback(c), flags(fl) {}
		std::string path;
		unsigned char* data;
		unsigned int size;
		void(*func)(void*, unsigned int, void*);
		void* param;
		void(*callback)();
		unsigned int flags;
	};

	struct sAsyncRead2
	{
		sAsyncRead2(std::string const& p, void* (*f)(void*, unsigned int, void*), void* par, void(*c)(void*), unsigned int fl) :path(p), func(f), param(par), callback(c), flags(fl) {}
		std::string path;
		unsigned char* data;
		unsigned int size;
		void* (*func)(void*, unsigned int, void*);
		void* param;
		void(*callback)(void*);
		unsigned int flags;
	};
public:
	void QueueFunc(sRunFunc const& func)
	{
		std::lock_guard<std::mutex> lk(m_funcMutex);
		if (func.flags & FLAG_HIGH_PRIORITY)
		{
			m_funcs.push_front(func);
		}
		else
		{
			m_funcs.push_back(func);
		}
	}

	void QueueFunc(sRunFunc2 const& func)
	{
		std::lock_guard<std::mutex> lk(m_funcMutex2);
		if (func.flags & FLAG_HIGH_PRIORITY)
		{
			m_funcs2.push_front(func);
		}
		else
		{
			m_funcs2.push_back(func);
		}
	}

	void RunFunc(void* (*func)(void*), void* param, unsigned int flags)
	{
		m_cancelled = false;
		QueueFunc(sRunFunc2(func, param, NULL, flags));
	}

	void RunFunc(void(*func)(void*), void* param, void(*doneCallback)(), unsigned int flags)
	{
		m_cancelled = false;
		QueueFunc(sRunFunc(func, param, doneCallback, flags));
	}

	void RunFunc(void* (*func)(void*), void* param, void(*doneCallback)(void*), unsigned int flags)
	{
		m_cancelled = false;
		QueueFunc(sRunFunc2(func, param, doneCallback, flags));
	}

	void AsyncReadFile(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(), unsigned int flags)
	{
		m_cancelled = false;
		std::thread([=]{ ReadData(path, func, param, doneCallback, flags); }).detach();
	}

	void AsyncReadFile(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*), unsigned int flags)
	{
		m_cancelled = false;
		std::thread([=]{ ReadData2(path, func, param, doneCallback, flags); }).detach();
	}

	void QueueCallback(void(*callback)())
	{
		std::lock_guard<std::mutex> lk(m_callbackMutex);
		m_callbacks.push_back(callback);
	}

	void QueueCallback(void(*callback)(void*), void *params)
	{
		std::lock_guard<std::mutex> lk(m_callbackMutex2);
		m_callbacks2.push_back(std::pair<void(*)(void*), void*>(callback, params));
	}

	void ReportThreadClose()
	{
		std::lock_guard<std::mutex> lk(m_threadsMutex);
		m_currentThreads--;
	}

	int GetWorkerTimeout()
	{
		return m_threadsTimeout;
	}

	void Update()
	{
		if ((!m_funcs.empty() || !m_funcs2.empty()) && m_currentThreads < m_maxThreads)
		{
			m_threadsMutex.lock();
			m_currentThreads++;
			m_threadsMutex.unlock();
			std::thread([this]{ WorkerThread(); }).detach();
		}
		while (!m_callbacks.empty())
		{
			if (m_callbacks.front()) m_callbacks.front()();
			bool last = m_callbacks.size() == 1;
			if (last) m_callbackMutex.lock();
			m_callbacks.pop_front();
			if (last) m_callbackMutex.unlock();
		}
		while (!m_callbacks2.empty())
		{
			if (m_callbacks2.front().first) m_callbacks2.front().first(m_callbacks2.front().second);
			bool last = m_callbacks2.size() == 1;
			if (last) m_callbackMutex2.lock();
			m_callbacks2.pop_front();
			if (last) m_callbackMutex2.unlock();
		}
		while(!m_taskCallbacks.empty())
		{
			m_taskCallbacks.front()();
			bool last = m_callbacks2.size() == 1;
			if (last) m_callbackMutex2.lock();
			m_callbacks2.pop_front();
			if (last) m_callbackMutex2.unlock();
		}
	}

	void WaitAll()
	{
		int timeout = GetWorkerTimeout();
		SetWorkerTimeout(0);
		m_threadsMutex.lock();
		m_currentThreads++;
		m_threadsMutex.unlock();
		WorkerThread();
		SetWorkerTimeout(timeout);
	}

	void SetWorkerTimeout(int timeout)
	{
		m_threadsTimeout = timeout;
	}

	void CancelAll()
	{
		m_cancelled = true;
		m_funcMutex.lock();
		m_funcs.clear();
		m_funcMutex.unlock();
		m_funcMutex2.lock();
		m_funcs2.clear();
		m_funcMutex2.unlock();
		m_callbackMutex.lock();
		m_callbacks.clear();
		m_callbackMutex.unlock();
		m_callbackMutex2.lock();
		m_callbacks2.clear();
		m_callbackMutex2.unlock();
	}

	void ReadData(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(), unsigned int flags)
	{
		FILE * file = fopen(path.c_str(), "rb");
		if (!file)
		{
			LogWriter::WriteLine("Cannot open file " + path);
			return;
		}
		sAsyncRead * run = new sAsyncRead(path, func, param, doneCallback, flags);
		fseek(file, 0L, SEEK_END);
		run->size = ftell(file);
		fseek(file, 0L, SEEK_SET);
		run->data = new unsigned char[run->size];
		fread(run->data, 1, run->size, file);
		fclose(file);
		if (m_cancelled)
		{
			return;
		}
		if (flags & FLAG_FAST_FUNCTION)
		{
			func(run->data, run->size, param);
			if (doneCallback)
				QueueCallback(doneCallback);
		}
		else
		{
			QueueFunc(sRunFunc(ProcessData, run, doneCallback, flags));
		}
	}

	void ReadData2(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*), unsigned int flags)
	{
		FILE * file = fopen(path.c_str(), "rb");
		if (!file)
		{
			LogWriter::WriteLine("Cannot open file " + path);
			return;
		}
		sAsyncRead2 * run = new sAsyncRead2(path, func, param, doneCallback, flags);
		fseek(file, 0L, SEEK_END);
		run->size = ftell(file);
		fseek(file, 0L, SEEK_SET);
		run->data = new unsigned char[run->size];
		fread(run->data, 1, run->size, file);
		fclose(file);
		if (m_cancelled)
		{
			return;
		}
		if (flags & FLAG_FAST_FUNCTION)
		{
			void * result = func(run->data, run->size, param);
			if (doneCallback)
				QueueCallback(doneCallback, result);
		}
		else
		{
			QueueFunc(sRunFunc2(ProcessData2, run, doneCallback, flags));
		}
	}

	void WorkerThread()
	{
		int timeUntilExit = GetWorkerTimeout();
		while (timeUntilExit > 0)
		{
			if (m_cancelled)
			{
				return;
			}
			timeUntilExit--;
			{
				std::unique_lock<std::mutex> lk(m_funcMutex);
				if (!m_funcs.empty())
				{
					sRunFunc func = std::move(m_funcs.front());
					m_funcs.pop_front();
					lk.unlock();
					func.func(func.param);
					if (func.callback)
					{
						QueueCallback(func.callback);
					}
					timeUntilExit = GetWorkerTimeout();
				}
			}
			{
				std::unique_lock<std::mutex> lk(m_funcMutex2);
				if (!m_funcs2.empty())
				{
					sRunFunc2 func = std::move(m_funcs2.front());
					m_funcs2.pop_front();
					lk.unlock();
					void* result = func.func(func.param);
					if (func.callback) QueueCallback(func.callback, result);
					timeUntilExit = GetWorkerTimeout();
				}
			}
			{
				std::unique_lock<std::mutex> lk(m_funcMutex2);
				if(!m_tasks.empty())
				{
					std::unique_ptr<ITask> task = std::move(m_tasks.front());
					m_tasks.pop_front();
					task->Execute();
				}
			}
			if (timeUntilExit < GetWorkerTimeout())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
		ReportThreadClose();
	}

	void AddTask(ITask & task)
	{
		m_tasks.push_back(std::unique_ptr<ITask>(&task));
	}

	void AddTaskCallback(std::function<void()> const& func)
	{
		m_taskCallbacks.push_back(func);
	}

	std::deque<void(*)()> m_callbacks;
	std::deque<std::pair<void(*)(void*), void*>> m_callbacks2;
	std::deque<sRunFunc> m_funcs;
	std::deque<sRunFunc2> m_funcs2;
	std::deque<std::unique_ptr<ITask>> m_tasks;
	std::deque<std::function<void()>> m_taskCallbacks;
	unsigned int m_currentThreads = 0;
	unsigned int m_maxThreads = std::thread::hardware_concurrency();
	int m_threadsTimeout = 2000;
	bool m_cancelled = false;
	std::mutex m_callbackMutex;
	std::mutex m_callbackMutex2;
	std::mutex m_funcMutex;
	std::mutex m_funcMutex2;
	std::mutex m_threadsMutex;
	std::mutex m_taskMutex;
};

void ProcessData(void* param)
{
	auto run = std::unique_ptr<ThreadPool::Impl::sAsyncRead>((ThreadPool::Impl::sAsyncRead*)param);
	run->func(run->data, run->size, run->param);
}

void* ProcessData2(void* param)
{
	auto run = std::unique_ptr<ThreadPool::Impl::sAsyncRead2>((ThreadPool::Impl::sAsyncRead2*)param);
	return run->func(run->data, run->size, run->param);
}

std::unique_ptr<ThreadPool::Impl> ThreadPool::m_pImpl(std::unique_ptr<ThreadPool::Impl>(new ThreadPool::Impl()));

void ThreadPool::RunFunc(void* (*func)(void*), void* param, unsigned int flags)
{
	m_pImpl->RunFunc(func, param, flags);
}

void ThreadPool::RunFunc(void(*func)(void*), void* param, void(*doneCallback)(), unsigned int flags)
{
	m_pImpl->RunFunc(func, param, doneCallback, flags);
}

void ThreadPool::RunFunc(void* (*func)(void*), void* param, void(*doneCallback)(void*), unsigned int flags)
{
	m_pImpl->RunFunc(func, param, doneCallback, flags);
}

void ThreadPool::AsyncReadFile(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(), unsigned int flags)
{
	m_pImpl->AsyncReadFile(path, func, param, doneCallback, flags);
}

void ThreadPool::AsyncReadFile(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*), unsigned int flags)
{
	m_pImpl->AsyncReadFile(path, func, param, doneCallback, flags);
}

void ThreadPool::Update()
{
	m_pImpl->Update();
}

void ThreadPool::WaitAll()
{
	m_pImpl->WaitAll();
}

int ThreadPool::GetWorkerTimeout()
{
	return m_pImpl->GetWorkerTimeout();
}

void ThreadPool::SetWorkerTimeout(int timeout)
{
	m_pImpl->SetWorkerTimeout(timeout);
}

void ThreadPool::CancelAll()
{
	m_pImpl->CancelAll();
}

void ThreadPool::AddTask(ITask & task)
{
	m_pImpl->AddTask(task);
}

void ThreadPool::AddTaskCallback(std::function<void()> const& func)
{
	m_pImpl->AddTaskCallback(func);
}