#include "ThreadPool.h"
#include "LogWriter.h"
#include <mutex>
#include <future>

#ifdef _WINDOWS
#include <Windows.h>
void StartThread(void* (*func)(void*), void* param)
{
	CreateThread(NULL, 65536, (LPTHREAD_START_ROUTINE)func, param, 0, NULL);
}

#elif ___unix___
#include <pthread.h>

void StartThread(void* (*func)(void*), void* param)
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int id = pthread_create(&thread, &attr, func, param);
	pthread_attr_destroy(&attr);
}

#endif

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
	void QueueFunc(sRunFunc * func)
	{
		csfunc.lock();
		if (func->flags & FLAG_HIGH_PRIORITY)
		{
			m_funcs.push_front(func);
		}
		else
		{
			m_funcs.push_back(func);
		}
		csfunc.unlock();
	}

	void QueueFunc(sRunFunc2 * func)
	{
		csfunc2.lock();
		if (func->flags & FLAG_HIGH_PRIORITY)
		{
			m_funcs2.push_front(func);
		}
		else
		{
			m_funcs2.push_back(func);
		}
		csfunc2.unlock();
	}

	void* Func(void* param)
	{
		sRunFunc * func = (sRunFunc *)param;
		func->func(func->param);
		if (func->callback)
			QueueCallback(func->callback);
		delete func;
		return 0;
	}

	void* Func2(void* param)
	{
		sRunFunc2 * func = (sRunFunc2 *)param;
		void * result = func->func(func->param);
		if (func->callback)
			QueueCallback(func->callback, result);
		delete func;
		return 0;
	}

	void RunFunc(void* (*func)(void*), void* param, unsigned int flags)
	{
		m_cancelled = false;
		QueueFunc(new sRunFunc2(func, param, NULL, flags));
	}

	void RunFunc(void(*func)(void*), void* param, void(*doneCallback)(), unsigned int flags)
	{
		m_cancelled = false;
		QueueFunc(new sRunFunc(func, param, doneCallback, flags));
	}

	void RunFunc(void* (*func)(void*), void* param, void(*doneCallback)(void*), unsigned int flags)
	{
		m_cancelled = false;
		QueueFunc(new sRunFunc2(func, param, doneCallback, flags));
	}

	void AsyncReadFile(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(), unsigned int flags)
	{
		m_cancelled = false;
		StartThread(ReadData, new sAsyncRead(path, func, param, doneCallback, flags));
	}

	void AsyncReadFile(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*), unsigned int flags)
	{
		m_cancelled = false;
		StartThread(ReadData2, new sAsyncRead2(path, func, param, doneCallback, flags));
	}

	void QueueCallback(void(*callback)())
	{
		cscallback.lock();
		m_callbacks.push_back(callback);
		cscallback.unlock();
	}

	void QueueCallback(void(*callback)(void*), void *params)
	{
		cscallback2.lock();
		m_callbacks2.push_back(std::pair<void(*)(void*), void*>(callback, params));
		cscallback2.unlock();
	}

	void ReportThreadClose()
	{
		cstp.lock();
		m_currentThreads--;
		cstp.unlock();
	}

	int GetWorkerTimeout()
	{
		return m_threadsTimeout;
	}

	void Update()
	{
		if ((!m_funcs.empty() || !m_funcs2.empty()) && m_currentThreads < m_maxThreads)
		{
			cstp.lock();
			m_currentThreads++;
			cstp.unlock();
			StartThread(WorkerThread, NULL);
		}
		while (!m_callbacks.empty())
		{
			if (m_callbacks.front()) m_callbacks.front()();
			bool last = m_callbacks.size() == 1;
			if (last) cscallback.lock();
			m_callbacks.pop_front();
			if (last) cscallback.unlock();
		}
		while (!m_callbacks2.empty())
		{
			if (m_callbacks2.front().first) m_callbacks2.front().first(m_callbacks2.front().second);
			bool last = m_callbacks2.size() == 1;
			if (last) cscallback2.lock();
			m_callbacks2.pop_front();
			if (last) cscallback2.unlock();
		}
	}

	void WaitAll()
	{
		int timeout = GetWorkerTimeout();
		SetWorkerTimeout(0);
		while (m_currentThreads > 0 || !m_funcs.empty() || !m_funcs2.empty())
		{
			csfunc.lock();
			if (!m_funcs.empty())
			{
				auto func = m_funcs.front();
				m_funcs.pop_front();
				csfunc.unlock();
				func->func(func->param);
				if (func->callback) QueueCallback(func->callback);
			}
			csfunc.unlock();
			csfunc2.lock();
			if (!m_funcs2.empty())
			{
				auto func = m_funcs2.front();
				m_funcs2.pop_front();
				csfunc2.unlock();
				void* result = func->func(func->param);
				if (func->callback) QueueCallback(func->callback, result);
			}
			csfunc2.unlock();
			Update();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		SetWorkerTimeout(timeout);
	}

	void SetWorkerTimeout(int timeout)
	{
		m_threadsTimeout = timeout;
	}

	void CancelAll()
	{
		m_cancelled = true;
		csfunc.lock();
		m_funcs.clear();
		csfunc.unlock();
		csfunc2.lock();
		m_funcs2.clear();
		csfunc2.unlock();
		cscallback.lock();
		m_callbacks.clear();
		cscallback.unlock();
		cscallback2.lock();
		m_callbacks2.clear();
		cscallback2.unlock();
	}
	std::list<void(*)()> m_callbacks;
	std::list<std::pair<void(*)(void*), void*>> m_callbacks2;
	std::list<sRunFunc*> m_funcs;
	std::list<sRunFunc2*> m_funcs2;
	unsigned int m_currentThreads = 0;
	unsigned int m_maxThreads = std::thread::hardware_concurrency();
	int m_threadsTimeout = 2000;
	bool m_cancelled = false;
	std::mutex cscallback;
	std::mutex cscallback2;
	std::mutex csfunc;
	std::mutex csfunc2;
	std::mutex cstp;
};

std::unique_ptr<ThreadPool::Impl> ThreadPool::m_pImpl(std::make_unique<ThreadPool::Impl>());

void* ThreadPool::ReadData(void* param)
{
	Impl::sAsyncRead* run = (Impl::sAsyncRead*)param;
	FILE * file = fopen(run->path.c_str(), "rb");
	if (!file)
	{
		LogWriter::WriteLine("Cannot open file " + run->path);
		delete run;
		return 0;
	}
	fseek(file, 0L, SEEK_END);
	run->size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	run->data = new unsigned char[run->size];
	fread(run->data, 1, run->size, file);
	fclose(file);
	if (m_pImpl->m_cancelled) return 0;
	if (run->flags & FLAG_FAST_FUNCTION)
	{
		run->func(run->data, run->size, run->param);
		if (run->callback)
			m_pImpl->QueueCallback(run->callback);
		delete run;
	}
	else
	{
		m_pImpl->QueueFunc(new Impl::sRunFunc(ProcessData, run, run->callback, run->flags));
	}
	return 0;
}

void* ThreadPool::ReadData2(void* param)
{
	Impl::sAsyncRead2* run = ((Impl::sAsyncRead2*)param);
	FILE * file = fopen(run->path.c_str(), "rb");
	if (!file)
	{
		LogWriter::WriteLine("Cannot open file " + run->path);
		delete run;
		return 0;
	}
	fseek(file, 0L, SEEK_END);
	run->size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	run->data = new unsigned char[run->size];
	fread(run->data, 1, run->size, file);
	fclose(file);
	if (m_pImpl->m_cancelled) return 0;
	if (run->flags & FLAG_FAST_FUNCTION)
	{
		void * result = run->func(run->data, run->size, run->param);
		if (run->callback)
			m_pImpl->QueueCallback(run->callback, result);
		delete run;
	}
	else
	{
		m_pImpl->QueueFunc(new Impl::sRunFunc2(ProcessData2, run, run->callback, run->flags));
	}
	return 0;
}

void* ThreadPool::WorkerThread(void* param)
{
	int timeUntilExit = GetWorkerTimeout();
	while (timeUntilExit > 0)
	{
		if (m_pImpl->m_cancelled) return 0;
		timeUntilExit--;
		m_pImpl->csfunc.lock();
		if (!m_pImpl->m_funcs.empty())
		{
			auto func = m_pImpl->m_funcs.front();
			m_pImpl->m_funcs.pop_front();
			m_pImpl->csfunc.unlock();
			func->func(func->param);
			if (func->callback) m_pImpl->QueueCallback(func->callback);
			timeUntilExit = GetWorkerTimeout();
		}
		m_pImpl->csfunc.unlock();
		m_pImpl->csfunc2.lock();
		if (!m_pImpl->m_funcs2.empty())
		{
			auto func = m_pImpl->m_funcs2.front();
			m_pImpl->m_funcs2.pop_front();
			m_pImpl->csfunc2.unlock();
			void* result = func->func(func->param);
			if (func->callback) m_pImpl->QueueCallback(func->callback, result);
			timeUntilExit = GetWorkerTimeout();
		}
		else
		{
			m_pImpl->csfunc2.unlock();
		}
		if (timeUntilExit < GetWorkerTimeout())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	m_pImpl->ReportThreadClose();
	return NULL;
}

void ThreadPool::ProcessData(void* param)
{
	Impl::sAsyncRead* run = (Impl::sAsyncRead*)param;
	run->func(run->data, run->size, run->param);
	delete run;
}

void* ThreadPool::ProcessData2(void* param)
{
	Impl::sAsyncRead2* run = (Impl::sAsyncRead2*)param;
	void* result = run->func(run->data, run->size, run->param);
	delete run;
	return result;
}

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