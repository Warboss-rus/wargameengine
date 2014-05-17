#include "ThreadPool.h"
#include "Threading.h"
#include "LogWriter.h"

std::list<void(*)()> ThreadPool::m_callbacks;
std::list<std::pair<void(*)(void*), void*>> ThreadPool::m_callbacks2;
std::list<sRunFunc*> ThreadPool::m_funcs;
std::list<sRunFunc2*> ThreadPool::m_funcs2;
CCriticalSection cscallback;
CCriticalSection cscallback2;
CCriticalSection csfunc;
CCriticalSection csfunc2;
int ThreadPool::m_currentThreads = 0;
int ThreadPool::m_maxThreads = CoresCount();

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
	((sRunFunc*)param)->func(((sRunFunc*)param)->param);
	if (((sRunFunc*)param)->callback)
		QueueCallback(((sRunFunc*)param)->callback);
	delete (sRunFunc*)param;
	return 0;
}

void* ThreadPool::Func2(void* param)
{
	void * result = ((sRunFunc2*)param)->func(((sRunFunc2*)param)->param);
	if (((sRunFunc*)param)->callback)
		QueueCallback(((sRunFunc2*)param)->callback, result);
	delete (sRunFunc*)param;
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
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	unsigned char* data = new unsigned char[size];
	fread(data, 1, size, file);
	fclose(file);
	run->func(data, size, run->param);
	if (run->callback)
		QueueCallback(run->callback);
	delete run;
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
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	unsigned char* data = new unsigned char[size];
	fread(data, 1, size, file);
	fclose(file);
	void * result = run->func(data, size, run->param);
	if (run->callback)
		QueueCallback(run->callback, result);
	delete run;
	return 0;
}

void ThreadPool::RunFunc(void* (*func)(void*), void* param)
{
	StartThread(func, param);
}

void ThreadPool::RunFunc(void(*func)(void*), void* param, void(*doneCallback)())
{
	StartThread(Func, new sRunFunc(func, param, doneCallback));
}

void ThreadPool::RunFunc(void* (*func)(void*), void* param, void(*doneCallback)(void*))
{
	StartThread(Func, new sRunFunc2(func, param, doneCallback));
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
	csfunc.Enter();
	m_funcs2.push_back(func);
	csfunc.Leave();
}

void ThreadPool::Update()
{
	if ((!m_funcs.empty() || !m_funcs2.empty()) && m_currentThreads < m_maxThreads)
	{
		m_currentThreads++;
		StartThread(Func, m_funcs.front());
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