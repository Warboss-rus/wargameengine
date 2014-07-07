#include "ThreadPool.h"
#include "LogWriter.h"

std::list<void(*)()> ThreadPool::m_callbacks;
std::list<std::pair<void(*)(void*), void*>> ThreadPool::m_callbacks2;
std::list<sRunFunc*> ThreadPool::m_funcs;
std::list<sRunFunc2*> ThreadPool::m_funcs2;
int ThreadPool::m_currentThreads = 0;
int ThreadPool::m_maxThreads = CoresCount();
int ThreadPool::m_threadsTimeout = 2000;
CCriticalSection cscallback;
CCriticalSection cscallback2;
CCriticalSection csfunc;
CCriticalSection csfunc2;
CCriticalSection cstp;

struct sRunFunc
{
	sRunFunc(void(*f)(void*), void* p, void(*c)()) :func(f), param(p), callback(c) {}
	void(*func)(void*);
	void* param;
	void(*callback)();
};

struct sRunFunc2
{
	sRunFunc2(void* (*f)(void*), void* p, void(*c)(void*)) :func(f), param(p), callback(c) {}
	void* (*func)(void*);
	void* param;
	void(*callback)(void*);
};

struct sAsyncRead
{
	sAsyncRead(std::string const& p, void(*f)(void*, unsigned int, void*), void* par, void(*c)()) :path(p), func(f), param(par), callback(c) {}
	std::string path;
	unsigned char* data;
	unsigned int size;
	void(*func)(void*, unsigned int, void*);
	void* param;
	void(*callback)();
};

struct sAsyncRead2
{
	sAsyncRead2(std::string const& p, void* (*f)(void*, unsigned int, void*), void* par, void(*c)(void*)) :path(p), func(f), param(par), callback(c) {}
	std::string path;
	unsigned char* data;
	unsigned int size;
	void* (*func)(void*, unsigned int, void*);
	void* param;
	void(*callback)(void*);
};

void* ThreadPool::Func(void* param)
{
	sRunFunc * func = (sRunFunc *)param;
	func->func(func->param);
	if (func->callback)
		QueueCallback(func->callback);
	delete func;
	return 0;
}

void* ThreadPool::Func2(void* param)
{
	sRunFunc2 * func = (sRunFunc2 *)param;
	void * result = func->func(func->param);
	if (func->callback)
		QueueCallback(func->callback, result);
	delete func;
	return 0;
}

void* ThreadPool::ReadData(void* param)
{
	sAsyncRead* run = (sAsyncRead*)param;
	FILE * file = fopen(run->path.c_str(), "rb");
	if (!file)
	{
		CLogWriter::WriteLine("Cannot open file " + run->path);
		delete run;
		return 0;
	}
	fseek(file, 0L, SEEK_END);
	run->size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	run->data = new unsigned char[run->size];
	fread(run->data, 1, run->size, file);
	fclose(file);
#ifdef _SPEED_HACK
	run->func(run->data, run->size, run->param);
	if (run->callback)
		QueueCallback(run->callback);
	delete run;
#else
	QueueFunc(new sRunFunc(ProcessData, run, run->callback));
#endif
	return 0;
}

void* ThreadPool::ReadData2(void* param)
{
	sAsyncRead2* run = ((sAsyncRead2*)param);
	FILE * file = fopen(run->path.c_str(), "rb");
	if (!file)
	{
		CLogWriter::WriteLine("Cannot open file " + run->path);
		delete run;
		return 0;
	}
	fseek(file, 0L, SEEK_END);
	run->size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	run->data = new unsigned char[run->size];
	fread(run->data, 1, run->size, file);
	fclose(file);
#ifdef _SPEED_HACK
	void * result = run->func(run->data, run->size, run->param);
	if (run->callback)
		QueueCallback(run->callback, result);
	delete run;
#else
	QueueFunc(new sRunFunc2(ProcessData2, run, run->callback));
#endif
	return 0;
}

void ThreadPool::ProcessData(void* param)
{
	sAsyncRead* run = (sAsyncRead*)param;
	run->func(run->data, run->size, run->param);
	delete run;
}

void* ThreadPool::ProcessData2(void* param)
{
	sAsyncRead2* run = (sAsyncRead2*)param;
	void* result = run->func(run->data, run->size, run->param);
	delete run;
	return result;
}

void* ThreadPool::WorkerThread(void* param)
{
	int timeUntilExit = GetWorkerTimeout();
	while (timeUntilExit > 0)
	{
		timeUntilExit--;
		csfunc.Enter();
		if (!m_funcs.empty())
		{
			auto func = m_funcs.front();
			m_funcs.pop_front();
			csfunc.Leave();
			func->func(func->param);
			if (func->callback) QueueCallback(func->callback);
			timeUntilExit = GetWorkerTimeout();
		}
		csfunc.Leave();
		csfunc2.Enter();
		if (!m_funcs2.empty())
		{
			auto func = m_funcs2.front();
			m_funcs2.pop_front();
			csfunc2.Leave();
			void* result = func->func(func->param);
			if (func->callback) QueueCallback(func->callback, result);
			timeUntilExit = GetWorkerTimeout();
		}
		csfunc2.Leave();
		if (timeUntilExit != GetWorkerTimeout()) Sleep(1);
	}
	ReportThreadClose();
	return NULL;
}

void ThreadPool::RunFunc(void* (*func)(void*), void* param)
{
	QueueFunc(new sRunFunc2(func, param, NULL));
}

void ThreadPool::RunFunc(void(*func)(void*), void* param, void(*doneCallback)())
{
	QueueFunc(new sRunFunc(func, param, doneCallback));
}

void ThreadPool::RunFunc(void* (*func)(void*), void* param, void(*doneCallback)(void*))
{
	QueueFunc(new sRunFunc2(func, param, doneCallback));
}

void ThreadPool::AsyncReadFile(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)())
{
	StartThread(ReadData, new sAsyncRead(path, func, param, doneCallback));
}

void ThreadPool::AsyncReadFile(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*))
{
	StartThread(ReadData2, new sAsyncRead2(path, func, param, doneCallback));
}

void ThreadPool::QueueCallback(void(*callback)())
{
	cscallback.Enter();
	m_callbacks.push_back(callback);
	cscallback.Leave();
}

void ThreadPool::QueueCallback(void(*callback)(void*), void *params)
{
	cscallback2.Enter();
	m_callbacks2.push_back(std::pair<void(*)(void*), void*>(callback, params));
	cscallback2.Leave();
}

void ThreadPool::QueueFunc(sRunFunc * func)
{
	csfunc.Enter();
	m_funcs.push_back(func);
	csfunc.Leave();
}

void ThreadPool::QueueFunc(sRunFunc2 * func)
{
	csfunc2.Enter();
	m_funcs2.push_back(func);
	csfunc2.Leave();
}

void ThreadPool::ReportThreadClose()
{
	cstp.Enter();
	m_currentThreads--;
	cstp.Leave();
}

int ThreadPool::GetWorkerTimeout()
{
	return m_threadsTimeout;
}

void ThreadPool::Update()
{
	if ((!m_funcs.empty() || !m_funcs2.empty()) && m_currentThreads < m_maxThreads)
	{
		cstp.Enter();
		m_currentThreads++;
		cstp.Leave();
		StartThread(WorkerThread, NULL);
	}
	while (!m_callbacks.empty())
	{
		if (m_callbacks.front()) m_callbacks.front()();
		bool last = m_callbacks.size() == 1;
		if (last) cscallback.Enter();
		m_callbacks.pop_front();
		if (last) cscallback.Leave();
	}
	while (!m_callbacks2.empty())
	{
		if (m_callbacks2.front().first) m_callbacks2.front().first(m_callbacks2.front().second);
		bool last = m_callbacks2.size() == 1;
		if (last) cscallback2.Enter();
		m_callbacks2.pop_front();
		if (last) cscallback2.Leave();
	}
}