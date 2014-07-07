#ifdef _WINDOWS
#include <Windows.h>
#elif ___unix___
#include <pthread.h>
#endif

void StartThread(void* (*func)(void*), void* param);
int CoresCount();
class CCriticalSection
{
public:
	CCriticalSection();
	~CCriticalSection();
	void Enter();
	void Leave();
private:
#ifdef _WINDOWS
	CRITICAL_SECTION m_cs;
#elif ___unix___
	pthread_mutex_t m_cs;
#else
#endif
};