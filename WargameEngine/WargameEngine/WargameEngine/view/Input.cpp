#include "Input.h"
#include <map>
#include "../Signal.h"

struct CInput::sSignals
{
	CSignal<int, int> m_onLMBDown;
	CSignal<int, int> m_onLMBUp;
	CSignal<int, int> m_onRMBDown;
	CSignal<int, int> m_onRMBUp;
	CSignal<> m_onWheelUp;
	CSignal<> m_onWheelDown;
	CSignal<int, int> m_onKeyDown;
	CSignal<int, int> m_onKeyUp;
	CSignal<unsigned int> m_onCharacter;
	CSignal<int, int> m_onMouseMove;
};

std::unique_ptr<CInput::sSignals> CInput::m_signals = std::make_unique<CInput::sSignals>();
bool CInput::m_cursorEnabled = true;
static const int SCROLL_UP = 3;
static const int SCROLL_DOWN = 4;
static int g_prevX;
static int g_prevY;

static const std::map<int, IInput::Modifiers> modifiersMap = {
	{ GLFW_MOD_ALT, IInput::MODIFIER_ALT },
	{ GLFW_MOD_CONTROL , IInput::MODIFIER_CTRL },
	{ GLFW_MOD_SHIFT , IInput::MODIFIER_SHIFT },
};

void CInput::OnMouse(GLFWwindow* window, int button, int action, int /*mods*/)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS)
		{
			m_signals->m_onLMBDown(x, y);
		}
		else
		{
			m_signals->m_onLMBUp(x, y);
		}break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS)
		{
			m_signals->m_onRMBDown(x, y);
		}
		else
		{
			m_signals->m_onRMBUp(x, y);
		}break;
	}
}

void CInput::OnScroll(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
{
	static const double offsetthreshold = 0.1;
	if (yoffset < -offsetthreshold)
	{
		m_signals->m_onWheelDown();
	}
	if (yoffset > offsetthreshold)
	{
		m_signals->m_onWheelUp();
	}
}

void CInput::OnKeyboard(GLFWwindow* /*window*/, int /*key*/, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		m_signals->m_onKeyUp(scancode, mods);
	}
	else
	{
		m_signals->m_onKeyDown(scancode, mods);
	}
}

void CInput::OnCharacter(GLFWwindow* /*window*/, unsigned int key)
{
	m_signals->m_onCharacter(key);
}

CInput::CInput(GLFWwindow * window)
	: m_window(window)
{
	m_signals = std::make_unique<CInput::sSignals>();
}

void CInput::DoOnLMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onLMBDown.Connect(handler, priority, tag);
}

void CInput::DoOnLMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onLMBUp.Connect(handler, priority, tag);
}

void CInput::DoOnRMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onRMBDown.Connect(handler, priority, tag);
}

void CInput::DoOnRMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onRMBUp.Connect(handler, priority, tag);
}

void CInput::DoOnMouseWheelUp(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onWheelUp.Connect(handler, priority, tag);
}

void CInput::DoOnMouseWheelDown(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onWheelDown.Connect(handler, priority, tag);
}

void CInput::DoOnKeyDown(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onKeyDown.Connect(handler, priority, tag);
}

void CInput::DoOnKeyUp(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onKeyUp.Connect(handler, priority, tag);
}

void CInput::DoOnCharacter(std::function<bool(unsigned int character) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onCharacter.Connect(handler, priority, tag);
}

void CInput::DoOnMouseMove(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onMouseMove.Connect(handler, priority, tag);
}

static int screenCenterX = 320;
static int screenCenterY = 240;
static int g_lastMouseX = screenCenterX;
static int g_lastMouseY = screenCenterY;

void CInput::EnableCursor(bool enable /*= true*/)
{
	glfwSetInputMode(m_window, GLFW_CURSOR, enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	m_cursorEnabled = enable;
}

void CInput::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	m_signals->m_onMouseMove(xpos, ypos);
}

int CInput::GetMouseX() const
{
	return g_lastMouseX;
}

int CInput::GetMouseY() const
{
	return g_lastMouseY;
}

void CInput::DeleteAllSignalsByTag(std::string const& tag)
{
	m_signals->m_onLMBDown.RemoveByTag(tag);
	m_signals->m_onLMBUp.RemoveByTag(tag);
	m_signals->m_onRMBDown.RemoveByTag(tag);
	m_signals->m_onRMBUp.RemoveByTag(tag);
	m_signals->m_onWheelUp.RemoveByTag(tag);
	m_signals->m_onWheelDown.RemoveByTag(tag);
	m_signals->m_onKeyDown.RemoveByTag(tag);
	m_signals->m_onKeyUp.RemoveByTag(tag);
	m_signals->m_onCharacter.RemoveByTag(tag);
	m_signals->m_onMouseMove.RemoveByTag(tag);
}

int CInput::GetModifiers() const
{
	return 0;//::GetModifiers();
}
