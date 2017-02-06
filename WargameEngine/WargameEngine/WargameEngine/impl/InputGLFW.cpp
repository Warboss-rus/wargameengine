#include "InputGLFW.h"
#include <map>
#include "../Signal.h"

CInputGLFW* CInputGLFW::m_instance = nullptr;
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
			m_instance->OnLMBDown(ix, iy);
		}
		else
		{
			m_instance->OnLMBUp(ix, iy);
		}break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS)
		{
			m_instance->OnRMBDown(ix, iy);
		}
		else
		{
			m_instance->OnRMBUp(ix, iy);
		}break;
	}
}

void CInputGLFW::OnScroll(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
{
	static const double offsetthreshold = 0.1;
	if (yoffset < -offsetthreshold)
	{
		m_instance->OnMouseWheelDown();
	}
	if (yoffset > offsetthreshold)
	{
		m_instance->OnMouseWheelUp();
	}
}

void CInputGLFW::OnKeyboard(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		m_instance->OnKeyUp(key, mods);
	}
	else
	{
		m_instance->OnKeyDown(key, mods);
	}
	m_instance->m_modifiers = mods;
}

void CInputGLFW::CharacterCallback(GLFWwindow* /*window*/, unsigned int key)
{
	m_instance->OnCharacter(static_cast<wchar_t>(key));
}

CInputGLFW::CInputGLFW(GLFWwindow * window)
	: m_window(window)
{
	m_instance = this;
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
		g_lastMouseX = screenCenterX;
		g_lastMouseY = screenCenterY;
		glfwSetCursorPos(m_window, screenCenterX, screenCenterY);
	}
}

void CInputGLFW::MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_instance->OnMouseMove(static_cast<int>(xpos), static_cast<int>(ypos));
	if (!m_instance->m_cursorEnabled)
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

void CInputGLFW::JoystickCallback(int joy, int event)
{
	if (event == GLFW_CONNECTED)
	{
		m_instance->m_activeJoysticks.push_back(joy);
	}
	else if (event == GLFW_DISCONNECTED)
	{
		m_instance->m_activeJoysticks.erase(std::find(m_instance->m_activeJoysticks.begin(), m_instance->m_activeJoysticks.end(), joy));
	}
}

void CInputGLFW::UpdateControllers()
{
	for (auto& joy : m_activeJoysticks)
	{
		if (m_gamepadStates.size() <= static_cast<size_t>(joy))
		{
			m_gamepadStates.resize(static_cast<size_t>(joy + 1));
		}
		auto& oldState = m_gamepadStates[joy];
		sControllerState state;
		int count;
		const unsigned char* buttons = glfwGetJoystickButtons(joy, &count);
		state.buttons.assign(buttons, buttons + count);
		if (oldState.buttons.size() <= static_cast<size_t>(count))
		{
			oldState.buttons.resize(static_cast<size_t>(count));
		}
		for (int j = 0; j < count; ++j)
		{

			if (buttons[j] != oldState.buttons[j])
			{
				m_instance->OnGamepadButton(joy, j, buttons[j] == GL_TRUE);
			}
		}
		const float* axes = glfwGetJoystickAxes(joy, &count);
		state.axes.assign(axes, axes + count);
		if (oldState.axes.size() <= static_cast<size_t>(count))
		{
			oldState.axes.resize(static_cast<size_t>(count));
		}
		for (int j = 0; j < count; j += 2)
		{
			if (axes[j] != oldState.axes[j] || (count > j + 1 && axes[j + 1] != oldState.axes[j + 1]))
			{
				m_instance->OnGamepadAxis(joy, j / 2, axes[j], axes[j + 1]);
			}
		}
		m_gamepadStates[joy] = state;
	}
}

void CInputGLFW::SetHeadRotation(int deviceIndex, float x, float y, float z)
{
	CInputBase::OnHeadRotation(deviceIndex, x, y, z);
}

int CInputGLFW::GetMouseX() const
{
	return g_lastMouseX;
}

int CInputGLFW::GetMouseY() const
{
	return g_lastMouseY;
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
	if (m_instance->m_modifiers & GLFW_MOD_ALT) result |= MODIFIER_ALT;
	if (m_instance->m_modifiers & GLFW_MOD_SHIFT) result |= MODIFIER_SHIFT;
	if (m_instance->m_modifiers & GLFW_MOD_CONTROL) result |= MODIFIER_CTRL;
	return result;
}
