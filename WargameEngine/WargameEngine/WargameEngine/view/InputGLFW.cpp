#include "InputGLFW.h"
#include <map>
#include "../Signal.h"

struct CInputGLFW::sSignals
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
	CSignal<int, int, bool> m_onGamepadButton;
	CSignal<int, int, double, double> m_onGamepadAxis;
	int m_modifiers;
};

std::unique_ptr<CInputGLFW::sSignals> CInputGLFW::m_signals = std::make_unique<CInputGLFW::sSignals>();
bool CInputGLFW::m_cursorEnabled = true;
static const int SCROLL_UP = 3;
static const int SCROLL_DOWN = 4;
static int g_prevX;
static int g_prevY;

static const std::map<int, IInput::Modifiers> modifiersMap = {
	{ GLFW_MOD_ALT, IInput::MODIFIER_ALT },
	{ GLFW_MOD_CONTROL , IInput::MODIFIER_CTRL },
	{ GLFW_MOD_SHIFT , IInput::MODIFIER_SHIFT },
};

void CInputGLFW::OnMouse(GLFWwindow* window, int button, int action, int /*mods*/)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	int ix = static_cast<int>(x);
	int iy = static_cast<int>(y);
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS)
		{
			m_signals->m_onLMBDown(ix, iy);
		}
		else
		{
			m_signals->m_onLMBUp(ix, iy);
		}break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS)
		{
			m_signals->m_onRMBDown(ix, iy);
		}
		else
		{
			m_signals->m_onRMBUp(ix, iy);
		}break;
	}
}

void CInputGLFW::OnScroll(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
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

void CInputGLFW::OnKeyboard(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		m_signals->m_onKeyUp(key, mods);
	}
	else
	{
		m_signals->m_onKeyDown(key, mods);
	}
	m_signals->m_modifiers = mods;
}

void CInputGLFW::OnCharacter(GLFWwindow* /*window*/, unsigned int key)
{
	m_signals->m_onCharacter(key);
}

CInputGLFW::CInputGLFW(GLFWwindow * window)
	: m_window(window)
{
	m_signals = std::make_unique<CInputGLFW::sSignals>();
}

void CInputGLFW::DoOnLMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onLMBDown.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnLMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onLMBUp.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnRMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onRMBDown.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnRMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onRMBUp.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnMouseWheelUp(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onWheelUp.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnMouseWheelDown(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onWheelDown.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnKeyDown(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onKeyDown.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnKeyUp(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onKeyUp.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnCharacter(std::function<bool(unsigned int character) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onCharacter.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnMouseMove(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_signals->m_onMouseMove.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnGamepadButtonStateChange(std::function<bool(int gamepadIndex, int buttonIndex, bool newState)> const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_signals->m_onGamepadButton.Connect(handler, priority, tag);
}

void CInputGLFW::DoOnGamepadAxisChange(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)> const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_signals->m_onGamepadAxis.Connect(handler, priority, tag);
}

static int screenCenterX = 320;
static int screenCenterY = 240;
static int g_lastMouseX = screenCenterX;
static int g_lastMouseY = screenCenterY;

void CInputGLFW::EnableCursor(bool enable /*= true*/)
{
	glfwSetInputMode(m_window, GLFW_CURSOR, enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	m_cursorEnabled = enable;
	if (enable)
	{
		glfwSetCursorPos(m_window, g_lastMouseX, g_lastMouseY);
	}
	else
	{
		g_prevX = g_lastMouseX;
		g_prevY = g_lastMouseY;
		int width, height;
		glfwGetWindowSize(m_window, &width, &height);
		screenCenterX = width / 2;
		screenCenterY = height / 2;
		glfwSetCursorPos(m_window, screenCenterX, screenCenterY);
		g_lastMouseX = screenCenterX;
		g_lastMouseY = screenCenterY;
	}
}

void CInputGLFW::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	m_signals->m_onMouseMove(static_cast<int>(xpos), static_cast<int>(ypos));
	if (!m_cursorEnabled)
	{
		if (g_prevX == 0 && g_prevY)
		{
			g_prevX = static_cast<int>(xpos);
			g_prevY = static_cast<int>(ypos);
		}
		glfwSetCursorPos(window, screenCenterX, screenCenterY);
	}
	else
	{
		g_lastMouseX = static_cast<int>(xpos);
		g_lastMouseY = static_cast<int>(ypos);
	}
}

void CInputGLFW::UpdateControllers()
{
	for (auto i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i)
	{
		if (glfwJoystickPresent(i) == GL_TRUE)
		{
			if (m_gamepadStates.size() <= static_cast<size_t>(i))
			{
				m_gamepadStates.resize(static_cast<size_t>(i + 1));
			}
			auto& oldState = m_gamepadStates[i];
			sControllerState state;
			int count;
			const unsigned char* buttons = glfwGetJoystickButtons(i, &count);
			state.buttons.assign(buttons, buttons + count);
			if (oldState.buttons.size() <= static_cast<size_t>(count))
			{
				oldState.buttons.resize(static_cast<size_t>(count));
			}
			for (int j = 0; j < count; ++j)
			{
				
				if (buttons[j] != oldState.buttons[j])
				{
					m_signals->m_onGamepadButton(i, j, buttons[j] == GL_TRUE);
				}
			}
			const float* axes = glfwGetJoystickAxes(i, &count);
			state.axes.assign(axes, axes + count);
			if (oldState.axes.size() <= static_cast<size_t>(count))
			{
				oldState.axes.resize(static_cast<size_t>(count));
			}
			for (int j = 0; j < count; j += 2)
			{
				if (axes[j] != oldState.axes[j] || (count > j + 1 && axes[j + 1] != oldState.axes[j + 1]))
				{
					m_signals->m_onGamepadAxis(i, j / 2, axes[j], axes[j+1]);
				}
			}
			m_gamepadStates[i] = state;
		}
	}
}

int CInputGLFW::GetMouseX() const
{
	return g_lastMouseX;
}

int CInputGLFW::GetMouseY() const
{
	return g_lastMouseY;
}

void CInputGLFW::DeleteAllSignalsByTag(std::string const& tag)
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

VirtualKey CInputGLFW::KeycodeToVirtualKey(int key) const
{
	static const std::map<int, VirtualKey> virtualKeys = {
		{ GLFW_KEY_BACKSPACE, KEY_BACKSPACE },
		{ GLFW_KEY_LEFT, KEY_LEFT },
		{ GLFW_KEY_UP, KEY_UP },
		{ GLFW_KEY_RIGHT, KEY_RIGHT },
		{ GLFW_KEY_DOWN, KEY_DOWN },
		{ GLFW_KEY_HOME, KEY_HOME },
		{ GLFW_KEY_END, KEY_END },
		{ GLFW_KEY_DELETE, KEY_DELETE },
	};
	auto it = virtualKeys.find(key);
	return it == virtualKeys.end() ? KEY_UNKNOWN : it->second;
}

int CInputGLFW::GetModifiers() const
{
	int result = 0;
	if (m_signals->m_modifiers & GLFW_MOD_ALT) result |= MODIFIER_ALT;
	if (m_signals->m_modifiers & GLFW_MOD_SHIFT) result |= MODIFIER_SHIFT;
	if (m_signals->m_modifiers & GLFW_MOD_CONTROL) result |= MODIFIER_CTRL;
	return result;
}
