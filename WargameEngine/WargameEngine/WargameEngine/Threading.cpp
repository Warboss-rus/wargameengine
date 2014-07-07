#include "Threading.h"
#ifdef _WINDOWS
void StartThread(void* (*func)(void*), void* param)
{
	CreateThread(NULL, 65536, (LPTHREAD_START_ROUTINE)func, param, 0, NULL);
}

int CoresCount() 
{ 
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}

CCriticalSection::CCriticalSection()
{
	InitializeCriticalSection(&m_cs);
}

CCriticalSection::~CCriticalSection()
{
	DeleteCriticalSection(&m_cs);
}

void CCriticalSection::Enter()
{
	EnterCriticalSection(&m_cs);
}

void CCriticalSection::Leave()
{
	LeaveCriticalSection(&m_cs);
}
#elif ___unix___
void StartThread(void* (*func)(void*), void* param)
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int id = pthread_create(&thread, &attr, func, param);
	pthread_attr_destroy(&attr);
}

int CoresCount() 
{
	return get_nprocs();
}

CCriticalSection::CCriticalSection()
{
	pthread_mutex_init(&m_cs, NULL);
}

CCriticalSection::~CCriticalSection()
{
	pthread_mutex_destroy(&m_cs);
}

void CCriticalSection::Enter()
{
	pthread_mutex_lock(&m_cs);
}

void CCriticalSection::Leave()
{
	pthread_mutex_unlock(&m_cs);
}
#else
void StartThread(void* (*func)(void*), void* param)
{
	func(param);
}
int CoresCount() { return 1; }
CCriticalSection::CCriticalSection(){}
CCriticalSection::~CCriticalSection(){}
void CCriticalSection::Enter(){}
void CCriticalSection::Leave(){}
#endif