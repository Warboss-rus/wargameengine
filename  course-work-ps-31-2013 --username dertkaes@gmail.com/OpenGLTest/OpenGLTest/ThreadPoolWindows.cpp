#include "ThreadPoolWindows.h"
#include <fstream>
#include <Windows.h>

std::list<void (*)()> CThreadPool::m_callbacks;
std::list<std::pair<void (*)(void*), void*>> CThreadPool::m_callbacks2;
CRITICAL_SECTION cs;

CThreadPool::CThreadPool()
{
	InitializeCriticalSection(&cs);
}

CThreadPool::~CThreadPool()
{
	DeleteCriticalSection(&cs);
}

struct sRunFunc
{
	sRunFunc(void (* f)(void*), void* p, void (* c)()):func(f), param(p), callback(c) {}
	void (* func)(void*);
	void* param;
	void (* callback)();
};

struct sRunFunc2
{
	sRunFunc2(void* (* f)(void*), void* p, void (* c)(void*)):func(f), param(p), callback(c) {}
	void* (* func)(void*);
	void* param;
	void (* callback)(void*);
};

struct sAsyncRead
{
	sAsyncRead(std::string const& p, void (* f)(void*, unsigned int, void*), void* par, void (* c)()):path(p), func(f), param(par), callback(c) {}
	std::string path;
	unsigned char* data;
	unsigned int size;
	void (* func)(void*, unsigned int, void*);
	void* param;
	void (* callback)();
};

struct sAsyncRead2
{
	sAsyncRead2(std::string const& p, void* (* f)(void*, unsigned int, void*), void* par, void (* c)(void*)):path(p), func(f), param(par), callback(c) {}
	std::string path;
	unsigned char* data;
	unsigned int size;
	void* (* func)(void*, unsigned int, void*);
	void* param;
	void (* callback)(void*);
};

DWORD WINAPI Func(LPVOID param)
{
	((sRunFunc*)param)->func(((sRunFunc*)param)->param);
	if(((sRunFunc*)param)->callback)
		CThreadPool::QueueCallback(((sRunFunc*)param)->callback);
	delete (sRunFunc*)param;
	return 0;
}

DWORD WINAPI Func2(LPVOID param)
{
	void * result = ((sRunFunc2*)param)->func(((sRunFunc2*)param)->param);
	if(((sRunFunc*)param)->callback)
		CThreadPool::QueueCallback(((sRunFunc2*)param)->callback, result);
	delete (sRunFunc*)param;
	return 0;
}

DWORD WINAPI ReadData(LPVOID param)
{
	sAsyncRead* run = (sAsyncRead*)param;
	FILE * file = fopen(run->path.c_str(), "rb");
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	unsigned char* data = new unsigned char[size];
	fread(data, 1, size, file);
	fclose(file);
	run->func(data, size, run->param);
	if(run->callback)
		CThreadPool::QueueCallback(run->callback);
	delete run;
	return 0;
}

DWORD WINAPI ReadData2(LPVOID param)
{
	sAsyncRead2* run = ((sAsyncRead2*)param);
	FILE * file = fopen(run->path.c_str(), "rb");
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	unsigned char* data = new unsigned char[size];
	fread(data, 1, size, file);
	fclose(file);
	void * result = run->func(data, size, run->param);
	if(run->callback)
		CThreadPool::QueueCallback(run->callback, result);
	delete run;
	return 0;
}

void CThreadPool::RunFunc(void (* func)(void*), void* param) 
{
	HANDLE thread = CreateThread(NULL, 65536, (LPTHREAD_START_ROUTINE)func, param, 0, NULL);
}

void CThreadPool::RunFunc(void (* func)(void*), void* param, void (* doneCallback)())
{
	HANDLE thread = CreateThread(NULL, 65536, Func, new sRunFunc(func, param, doneCallback), 0, NULL);
}

void CThreadPool::RunFunc(void* (* func)(void*), void* param, void (* doneCallback)(void*))
{
	HANDLE thread = CreateThread(NULL, 65536, Func, new sRunFunc2(func, param, doneCallback), 0, NULL);
}

void CThreadPool::AsyncReadFile(std::string const& path, void (* func)(void*, unsigned int, void*), void* param, void (* doneCallback)())
{
	HANDLE thread = CreateThread(NULL, 65536, ReadData, new sAsyncRead(path, func, param, doneCallback), 0, NULL);
}

void CThreadPool::AsyncReadFile(std::string const& path, void* (* func)(void*, unsigned int, void*), void* param, void (* doneCallback)(void*))
{
	HANDLE thread = CreateThread(NULL, 65536, ReadData2, new sAsyncRead2(path, func, param, doneCallback), 0, NULL);
}

void CThreadPool::QueueCallback(void (*callback)()) 
{ 
	EnterCriticalSection(&cs);
	m_callbacks.push_back(callback);
	LeaveCriticalSection(&cs);
}

void CThreadPool::QueueCallback(void (*callback)(void*), void *params) 
{ 
	EnterCriticalSection(&cs);
	m_callbacks2.push_back(std::pair<void (*)(void*), void*>(callback, params)); 
	LeaveCriticalSection(&cs);
}

void CThreadPool::Update()
{
	if(m_callbacks.empty() && m_callbacks2.empty()) return;
	EnterCriticalSection(&cs);
	while(!m_callbacks.empty())
	{
		if(m_callbacks.front()) m_callbacks.front()();
		m_callbacks.pop_front();
	}
	while(!m_callbacks2.empty())
	{
		if(m_callbacks2.front().first) m_callbacks2.front().first(m_callbacks2.front().second);
		m_callbacks2.pop_front();
	}
	LeaveCriticalSection(&cs);
}