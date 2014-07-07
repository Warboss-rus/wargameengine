#include "ThreadPool.h"

template<class X, class Y>
class Task
{
public:
	Task(X(*func)(Y), Y param) :m_func(func), m_param(param), m_state(CREATED){}
	void Run() { m_state = WAITING;  ThreadPool::RunFunc(Func, this); }
	enum eState
	{
		CREATED,
		WAITING,
		WORKING,
		COMPLETE
	};
	short GetState() const { return m_state; }
	bool IsWaiting() const { return m_state == WAITING; }
	bool IsWorking() const { return m_state == WORKING; }
	bool IsComplete() const  { return m_state == COMPLETE; }
	X Result() const { return m_result; }
	void Wait() const { while (m_state == WORKING || m_state == WAITING) { Sleep(2); } }
private:
	X(*m_func)(Y);
	Y m_param;
	X m_result;
	char m_state;
	static void* Func(void* param)
	{
		Task* t = (Task*)param;
		t->m_state = WORKING;
		t->m_result = t->m_func(t->m_param);
		t->m_state = COMPLETE;
		return 0;
	}
};

template<class X, class Y>
void WaitAll(Task<X, Y>* tasks, int count)
{
	bool complete = false;
	while (!complete)
	{
		complete = true;
		for (int i = 0; i < count; ++i)
		{
			if (!tasks[i]->IsComplete()) complete = false;
		}
		if (!complete) Sleep(2);
	}
}

template<class X, class Y>
void WaitAny(Task<X, Y>* tasks, int count)
{
	bool complete = false;
	while (!complete)
	{
		complete = false;
		for (int i = 0; i < count; ++i)
		{
			if (tasks[i]->IsComplete()) complete = true;
		}
		if (!complete) Sleep(2);
	}
}