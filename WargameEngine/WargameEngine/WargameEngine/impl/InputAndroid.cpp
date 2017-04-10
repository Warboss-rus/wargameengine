#include "InputAndroid.h"

void CInputAndroid::HandleInput(AInputEvent* event)
{
	switch (AInputEvent_getType(event))
	{
	case AINPUT_EVENT_TYPE_MOTION:
	{
		switch (AInputEvent_getSource(event))
		{
		case AINPUT_SOURCE_TOUCHSCREEN:
		case AINPUT_SOURCE_MOUSE:
		case AINPUT_SOURCE_STYLUS:
		{
			m_lastX = AMotionEvent_getX(event, 0);
			m_lastY = AMotionEvent_getY(event, 0);
			int action = AKeyEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
			switch (action) {
			case AMOTION_EVENT_ACTION_DOWN:
				OnLMBDown(m_lastX, m_lastY);
				break;
			case AMOTION_EVENT_ACTION_UP:
				OnLMBUp(m_lastX, m_lastY);
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				OnMouseMove(m_lastX, m_lastY);
				break;
			}
		}break;
		}
	}break;
	case AINPUT_EVENT_TYPE_KEY:
		switch (AInputEvent_getSource(event))
		{
		case AINPUT_SOURCE_KEYBOARD:
			OnKeyDown(AKeyEvent_getKeyCode(event), 0);
		}
	}
}

void CInputAndroid::HandleMotionEvent(int action, float x, float y)
{
	m_lastX = x;
	m_lastY = y;
	action &= AMOTION_EVENT_ACTION_MASK;
	switch (action) {
	case AMOTION_EVENT_ACTION_DOWN:
		OnLMBDown(m_lastX, m_lastY);
		break;
	case AMOTION_EVENT_ACTION_UP:
		OnLMBUp(m_lastX, m_lastY);
		break;
	case AMOTION_EVENT_ACTION_MOVE:
		OnMouseMove(m_lastX, m_lastY);
		break;
	}
}

void CInputAndroid::EnableCursor(bool enable /*= true*/)
{
	//no cursor on android for now
}

int CInputAndroid::GetModifiers() const
{
	return 0;
}

int CInputAndroid::GetMouseX() const
{
	return 0;//Return last touch position
}

int CInputAndroid::GetMouseY() const
{
	return 0;//Return last touch position
}

VirtualKey CInputAndroid::KeycodeToVirtualKey(int key) const
{
	return KEY_UNKNOWN;
}
