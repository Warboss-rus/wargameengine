#include "ThreadPoolPOSIX.h"
#include <fstream>
#include <pthread.h>
#include "LogWriter.h"

std::list<void(*)()> CThreadPoolPOSIX::m_callbacks;
std::list<std::pair<void(*)(void*), void*>> CThreadPoolPOSIX::m_callbacks2;
pthread_mutex_t mutex;
CThreadPoolPOSIX tpool;

CThreadPoolPOSIX::CThreadPoolPOSIX()
{
	pthread_mutex_init(&mutex, NULL);
}

CThreadPoolPOSIX::~CThreadPoolPOSIX()
{
	pthread_mutex_destroy(&mutex);
}

struct sRunFunc
{
	sRunFunc(void(*f)(void*), void* p, void(*c)()) :func(f), param(p), callback(c) {}
	void (* func)(void *);
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

void* Func(void* param)
{
	((sRunFunc*)param)->func(((sRunFunc*)param)->param);
	if (((sRunFunc*)param)->callback)
		CThreadPoolPOSIX::QueueCallback(((sRunFunc*)param)->callback);
	delete (sRunFunc*)param;
	return NULL;
}

void* Func2(void* param)
{
	void * result = ((sRunFunc2*)param)->func(((sRunFunc2*)param)->param);
	if (((sRunFunc*)param)->callback)
		CThreadPoolPOSIX::QueueCallback(((sRunFunc2*)param)->callback, result);
	delete (sRunFunc*)param;
	return NULL;
}

void* ReadData(void* param)
{
	sAsyncRead* run = (sAsyncRead*)param;
	FILE * file = fopen(run->path.c_str(), "rb");
	if (!file)
	{
		CLogWriter::WriteLine("Cannot open file " + run->path);
		delete run;
		return NULL;
	}
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	unsigned char* data = new unsigned char[size];
	fread(data, 1, size, file);
	fclose(file);
	run->func(data, size, run->param);
	if (run->callback)
		CThreadPoolPOSIX::QueueCallback(run->callback);
	delete run;
	return NULL;
}

void* ReadData2(void* param)
{
	sAsyncRead2* run = ((sAsyncRead2*)param);
	FILE * file = fopen(run->path.c_str(), "rb");
	if (!file)
	{
		CLogWriter::WriteLine("Cannot open file " + run->path);
		delete run;
		return NULL;
	}
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	unsigned char* data = new unsigned char[size];
	fread(data, 1, size, file);
	fclose(file);
	void * result = run->func(data, size, run->param);
	if (run->callback)
		CThreadPoolPOSIX::QueueCallback(run->callback, result);
	delete run;
	return NULL;
}

void CThreadPoolPOSIX::RunFunc(void* (_cdecl* func)(void *), void* param)
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int id = pthread_create(&thread, &attr, func, param);
	pthread_attr_destroy(&attr);
}

void CThreadPoolPOSIX::RunFunc(void (_cdecl* func)(void *), void* param, void(*doneCallback)())
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int id = pthread_create(&thread, &attr, Func, new sRunFunc(func, param, doneCallback));
	pthread_attr_destroy(&attr);
}

void CThreadPoolPOSIX::RunFunc(void* (*func)(void*), void* param, void(*doneCallback)(void*))
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int id = pthread_create(&thread, &attr, Func2, new sRunFunc2(func, param, doneCallback));
	pthread_attr_destroy(&attr);
}

void CThreadPoolPOSIX::AsyncReadFile(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)())
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int id = pthread_create(&thread, &attr, ReadData, new sAsyncRead(path, func, param, doneCallback));
	pthread_attr_destroy(&attr);
}

void CThreadPoolPOSIX::AsyncReadFile(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*))
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int id = pthread_create(&thread, &attr, ReadData2, new sAsyncRead2(path, func, param, doneCallback));
	pthread_attr_destroy(&attr);
}

void CThreadPoolPOSIX::QueueCallback(void(*callback)())
{
	pthread_mutex_lock(&mutex);
	m_callbacks.push_back(callback);
	pthread_mutex_unlock(&mutex);
}

void CThreadPoolPOSIX::QueueCallback(void(*callback)(void*), void *params)
{
	pthread_mutex_lock(&mutex);
	m_callbacks2.push_back(std::pair<void(*)(void*), void*>(callback, params));
	pthread_mutex_unlock(&mutex);
}

void CThreadPoolPOSIX::Update()
{
	if (m_callbacks.empty() && m_callbacks2.empty()) return;
	pthread_mutex_lock(&mutex);
	while (!m_callbacks.empty())
	{
		if (m_callbacks.front()) m_callbacks.front()();
		m_callbacks.pop_front();
	}
	while (!m_callbacks2.empty())
	{
		if (m_callbacks2.front().first) m_callbacks2.front().first(m_callbacks2.front().second);
		m_callbacks2.pop_front();
	}
	pthread_mutex_unlock(&mutex);
}