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
				m_onLMBDown(m_lastX, m_lastY);
				break;
			case AMOTION_EVENT_ACTION_UP:
				m_onLMBUp(m_lastX, m_lastY);
				break;
			case AMOTION_EVENT_ACTION_MOVE:
				m_onMouseMove(m_lastX, m_lastY);
				break;
			}
		}break;
		}
	}break;
	case AINPUT_EVENT_TYPE_KEY:
		switch (AInputEvent_getSource(event))
		{
		case AINPUT_SOURCE_KEYBOARD:
			m_onKeyDown(AKeyEvent_getKeyCode(event), 0);
		}
	}
}

void CInputAndroid::DoOnLMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onLMBDown.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnLMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onLMBUp.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnRMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onRMBDown.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnRMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onRMBUp.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnMouseWheelUp(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onZoomIn.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnMouseWheelDown(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onZoomOut.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnKeyDown(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onKeyDown.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnKeyUp(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onKeyUp.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnCharacter(std::function<bool(unsigned int character) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onCharacter.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnMouseMove(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onMouseMove.Connect(handler, priority, tag);
}

void CInputAndroid::DoOnGamepadButtonStateChange(std::function<bool(int, int, bool) > const&, int /*= 0*/, std::string const& /*= ""*/)
{
	//no gamepad support on android for now
}

void CInputAndroid::DoOnGamepadAxisChange(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	//no gamepad support on android for now
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

void CInputAndroid::DeleteAllSignalsByTag(std::string const& tag)
{
	m_onLMBDown.RemoveByTag(tag);
	m_onLMBUp.RemoveByTag(tag);
	m_onRMBDown.RemoveByTag(tag);
	m_onRMBUp.RemoveByTag(tag);
	m_onZoomIn.RemoveByTag(tag);
	m_onZoomOut.RemoveByTag(tag);
	m_onKeyDown.RemoveByTag(tag);
	m_onKeyUp.RemoveByTag(tag);
	m_onCharacter.RemoveByTag(tag);
	m_onMouseMove.RemoveByTag(tag);
}

VirtualKey CInputAndroid::KeycodeToVirtualKey(int key) const
{
	return KEY_UNKNOWN;
}
