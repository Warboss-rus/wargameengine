#include "TimedCallback.h"
#include <GL/glut.h>
#include "LUAScriptHandler.h"

CTimedCallback * CTimedCallback::m_self = NULL;

void TimedFunc(int value)
{
	const sTimedCallback * cb  = CTimedCallback::GetInstance()->GetCallback(value);
	if(!cb) return;
	CLUAScript::CallFunction(cb->function);
	if(cb->time)
	{
		glutTimerFunc(cb->time, TimedFunc, value);
	}
}

unsigned int CTimedCallback::AddCallback(std::string const& callback, unsigned int time, bool repeat)
{
	for(unsigned int i = 1; i < UINT_MAX; ++i)
	{
		if(m_callbacks.find(i) == m_callbacks.end())
		{
			sTimedCallback cb;
			cb.function = callback;
			cb.time = (repeat)?time:0;
			m_callbacks[i] = cb;
			glutTimerFunc(time, TimedFunc, i);
			return i;
		}
	}
	throw std::exception();
}

void CTimedCallback::DeleteCallback(unsigned int index)
{
	m_callbacks.erase(index);
}

const sTimedCallback * CTimedCallback::GetCallback(unsigned int index) 
{ 
	auto callback = m_callbacks.find(index);
	if(callback != m_callbacks.end())
	{
		return &m_callbacks[index]; 
	}
	return NULL;
}

CTimedCallback * CTimedCallback::GetInstance()
{
	if(!m_self)
	{
		m_self = new CTimedCallback;
	}
	return m_self;
}

void CTimedCallback::FreeInstance()
{
	delete m_self;
	m_self = NULL;
}