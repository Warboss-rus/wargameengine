#include "InputGLFW.h"
#include "../Signal.h"
#include "GameWindowGLFW.h"
#include <map>

static const int SCROLL_UP = 3;
static const int SCROLL_DOWN = 4;

void CInputGLFW::OnMouse(GLFWwindow* window, int button, int action, int mods)
{
	auto& instance = reinterpret_cast<CGameWindowGLFW*>(glfwGetWindowUserPointer(window))->Input();
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	int ix = static_cast<int>(x);
	int iy = static_cast<int>(y);
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS)
		{
			instance.OnLMBDown(ix, iy);
		}
		else
		{
			instance.OnLMBUp(ix, iy);
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS)
		{
			instance.OnRMBDown(ix, iy);
		}
		else
		{
			instance.OnRMBUp(ix, iy);
		}
		break;
	}
	instance.m_modifiers = mods;
}

void CInputGLFW::OnScroll(GLFWwindow* window, double /*xoffset*/, double yoffset)
{
	auto& instance = reinterpret_cast<CGameWindowGLFW*>(glfwGetWindowUserPointer(window))->Input();
	constexpr double offsetthreshold = 0.1;
	if (abs(yoffset) > offsetthreshold)
	{
		instance.OnMouseWheel(static_cast<float>(yoffset));
	}
}

void CInputGLFW::OnKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto& instance = reinterpret_cast<CGameWindowGLFW*>(glfwGetWindowUserPointer(window))->Input();
	if (action == GLFW_RELEASE)
	{
		instance.OnKeyUp(KeycodeToVirtualKey(key), scancode);
	}
	else
	{
		instance.OnKeyDown(KeycodeToVirtualKey(key), scancode);
	}
	instance.m_modifiers = mods;
}

void CInputGLFW::CharacterCallback(GLFWwindow* window, unsigned int key)
{
	auto& instance = reinterpret_cast<CGameWindowGLFW*>(glfwGetWindowUserPointer(window))->Input();
	instance.OnCharacter(static_cast<wchar_t>(key));
}

CInputGLFW::CInputGLFW(GLFWwindow* window)
	: m_window(window)
{
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
		m_prevX = g_lastMouseX;
		m_prevY = g_lastMouseY;
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
	auto& instance = reinterpret_cast<CGameWindowGLFW*>(glfwGetWindowUserPointer(window))->Input();
	instance.OnMouseMove(static_cast<int>(xpos), static_cast<int>(ypos));
	if (!instance.m_cursorEnabled)
	{
		if (instance.m_prevX == 0 && instance.m_prevY)
		{
			instance.m_prevX = static_cast<int>(xpos);
			instance.m_prevY = static_cast<int>(ypos);
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
	auto& instance = reinterpret_cast<CGameWindowGLFW*>(glfwGetWindowUserPointer(glfwGetCurrentContext()))->Input();
	if (event == GLFW_CONNECTED)
	{
		instance.m_activeJoysticks.push_back(joy);
	}
	else if (event == GLFW_DISCONNECTED)
	{
		instance.m_activeJoysticks.erase(std::find(instance.m_activeJoysticks.begin(), instance.m_activeJoysticks.end(), joy));
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
				OnGamepadButton(joy, j, buttons[j] == GL_TRUE);
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
				OnGamepadAxis(joy, j / 2, axes[j], axes[j + 1]);
			}
		}
		m_gamepadStates[joy] = state;
	}
}

void CInputGLFW::SetHeadRotation(size_t deviceIndex, const float* matrix)
{
	InputBase::OnHeadRotation(deviceIndex, matrix);
}

int CInputGLFW::GetMouseX() const
{
	return g_lastMouseX;
}

int CInputGLFW::GetMouseY() const
{
	return g_lastMouseY;
}

wargameEngine::view::VirtualKey CInputGLFW::KeycodeToVirtualKey(int key)
{
	switch (key)
	{
	case (GLFW_KEY_SPACE):
		return VirtualKey::KEY_SPACE;
	case (GLFW_KEY_APOSTROPHE):
		return VirtualKey::KEY_APOSTROPHE;
	case (GLFW_KEY_COMMA):
		return VirtualKey::KEY_COMMA;
	case (GLFW_KEY_MINUS):
		return VirtualKey::KEY_MINUS;
	case (GLFW_KEY_PERIOD):
		return VirtualKey::KEY_PERIOD;
	case (GLFW_KEY_SLASH):
		return VirtualKey::KEY_SLASH;
	case (GLFW_KEY_0):
		return VirtualKey::KEY_0;
	case (GLFW_KEY_1):
		return VirtualKey::KEY_1;
	case (GLFW_KEY_2):
		return VirtualKey::KEY_2;
	case (GLFW_KEY_3):
		return VirtualKey::KEY_3;
	case (GLFW_KEY_4):
		return VirtualKey::KEY_4;
	case (GLFW_KEY_5):
		return VirtualKey::KEY_5;
	case (GLFW_KEY_6):
		return VirtualKey::KEY_6;
	case (GLFW_KEY_7):
		return VirtualKey::KEY_7;
	case (GLFW_KEY_8):
		return VirtualKey::KEY_8;
	case (GLFW_KEY_9):
		return VirtualKey::KEY_9;
	case (GLFW_KEY_SEMICOLON):
		return VirtualKey::KEY_SEMICOLON;
	case (GLFW_KEY_EQUAL):
		return VirtualKey::KEY_EQUAL;
	case (GLFW_KEY_A):
		return VirtualKey::KEY_A;
	case (GLFW_KEY_B):
		return VirtualKey::KEY_B;
	case (GLFW_KEY_C):
		return VirtualKey::KEY_C;
	case (GLFW_KEY_D):
		return VirtualKey::KEY_D;
	case (GLFW_KEY_E):
		return VirtualKey::KEY_E;
	case (GLFW_KEY_F):
		return VirtualKey::KEY_F;
	case (GLFW_KEY_G):
		return VirtualKey::KEY_G;
	case (GLFW_KEY_H):
		return VirtualKey::KEY_H;
	case (GLFW_KEY_I):
		return VirtualKey::KEY_I;
	case (GLFW_KEY_J):
		return VirtualKey::KEY_J;
	case (GLFW_KEY_K):
		return VirtualKey::KEY_K;
	case (GLFW_KEY_L):
		return VirtualKey::KEY_L;
	case (GLFW_KEY_M):
		return VirtualKey::KEY_M;
	case (GLFW_KEY_N):
		return VirtualKey::KEY_N;
	case (GLFW_KEY_O):
		return VirtualKey::KEY_O;
	case (GLFW_KEY_P):
		return VirtualKey::KEY_P;
	case (GLFW_KEY_Q):
		return VirtualKey::KEY_Q;
	case (GLFW_KEY_R):
		return VirtualKey::KEY_R;
	case (GLFW_KEY_S):
		return VirtualKey::KEY_S;
	case (GLFW_KEY_T):
		return VirtualKey::KEY_T;
	case (GLFW_KEY_U):
		return VirtualKey::KEY_U;
	case (GLFW_KEY_V):
		return VirtualKey::KEY_V;
	case (GLFW_KEY_W):
		return VirtualKey::KEY_W;
	case (GLFW_KEY_X):
		return VirtualKey::KEY_X;
	case (GLFW_KEY_Y):
		return VirtualKey::KEY_Y;
	case (GLFW_KEY_Z):
		return VirtualKey::KEY_Z;
	case (GLFW_KEY_LEFT_BRACKET):
		return VirtualKey::KEY_LEFT_BRACKET;
	case (GLFW_KEY_BACKSLASH):
		return VirtualKey::KEY_BACKSLASH;
	case (GLFW_KEY_RIGHT_BRACKET):
		return VirtualKey::KEY_RIGHT_BRACKET;
	case (GLFW_KEY_GRAVE_ACCENT):
		return VirtualKey::KEY_GRAVE_ACCENT;
	case (GLFW_KEY_WORLD_1):
		return VirtualKey::KEY_WORLD_1;
	case (GLFW_KEY_WORLD_2):
		return VirtualKey::KEY_WORLD_2;
	case (GLFW_KEY_ESCAPE):
		return VirtualKey::KEY_ESCAPE;
	case (GLFW_KEY_ENTER):
		return VirtualKey::KEY_ENTER;
	case (GLFW_KEY_TAB):
		return VirtualKey::KEY_TAB;
	case (GLFW_KEY_BACKSPACE):
		return VirtualKey::KEY_BACKSPACE;
	case (GLFW_KEY_INSERT):
		return VirtualKey::KEY_INSERT;
	case (GLFW_KEY_DELETE):
		return VirtualKey::KEY_DELETE;
	case (GLFW_KEY_RIGHT):
		return VirtualKey::KEY_RIGHT;
	case (GLFW_KEY_LEFT):
		return VirtualKey::KEY_LEFT;
	case (GLFW_KEY_DOWN):
		return VirtualKey::KEY_DOWN;
	case (GLFW_KEY_UP):
		return VirtualKey::KEY_UP;
	case (GLFW_KEY_PAGE_UP):
		return VirtualKey::KEY_PAGE_UP;
	case (GLFW_KEY_PAGE_DOWN):
		return VirtualKey::KEY_PAGE_DOWN;
	case (GLFW_KEY_HOME):
		return VirtualKey::KEY_HOME;
	case (GLFW_KEY_END):
		return VirtualKey::KEY_END;
	case (GLFW_KEY_CAPS_LOCK):
		return VirtualKey::KEY_CAPS_LOCK;
	case (GLFW_KEY_SCROLL_LOCK):
		return VirtualKey::KEY_SCROLL_LOCK;
	case (GLFW_KEY_NUM_LOCK):
		return VirtualKey::KEY_NUM_LOCK;
	case (GLFW_KEY_PRINT_SCREEN):
		return VirtualKey::KEY_PRINT_SCREEN;
	case (GLFW_KEY_PAUSE):
		return VirtualKey::KEY_PAUSE;
	case (GLFW_KEY_F1):
		return VirtualKey::KEY_F1;
	case (GLFW_KEY_F2):
		return VirtualKey::KEY_F2;
	case (GLFW_KEY_F3):
		return VirtualKey::KEY_F3;
	case (GLFW_KEY_F4):
		return VirtualKey::KEY_F4;
	case (GLFW_KEY_F5):
		return VirtualKey::KEY_F5;
	case (GLFW_KEY_F6):
		return VirtualKey::KEY_F6;
	case (GLFW_KEY_F7):
		return VirtualKey::KEY_F7;
	case (GLFW_KEY_F8):
		return VirtualKey::KEY_F8;
	case (GLFW_KEY_F9):
		return VirtualKey::KEY_F9;
	case (GLFW_KEY_F10):
		return VirtualKey::KEY_F10;
	case (GLFW_KEY_F11):
		return VirtualKey::KEY_F11;
	case (GLFW_KEY_F12):
		return VirtualKey::KEY_F12;
	case (GLFW_KEY_KP_0):
		return VirtualKey::KEY_KP_0;
	case (GLFW_KEY_KP_1):
		return VirtualKey::KEY_KP_1;
	case (GLFW_KEY_KP_2):
		return VirtualKey::KEY_KP_2;
	case (GLFW_KEY_KP_3):
		return VirtualKey::KEY_KP_3;
	case (GLFW_KEY_KP_4):
		return VirtualKey::KEY_KP_4;
	case (GLFW_KEY_KP_5):
		return VirtualKey::KEY_KP_5;
	case (GLFW_KEY_KP_6):
		return VirtualKey::KEY_KP_6;
	case (GLFW_KEY_KP_7):
		return VirtualKey::KEY_KP_7;
	case (GLFW_KEY_KP_8):
		return VirtualKey::KEY_KP_8;
	case (GLFW_KEY_KP_9):
		return VirtualKey::KEY_KP_9;
	case (GLFW_KEY_KP_DECIMAL):
		return VirtualKey::KEY_KP_DECIMAL;
	case (GLFW_KEY_KP_DIVIDE):
		return VirtualKey::KEY_KP_DIVIDE;
	case (GLFW_KEY_KP_MULTIPLY):
		return VirtualKey::KEY_KP_MULTIPLY;
	case (GLFW_KEY_KP_SUBTRACT):
		return VirtualKey::KEY_KP_SUBTRACT;
	case (GLFW_KEY_KP_ADD):
		return VirtualKey::KEY_KP_ADD;
	case (GLFW_KEY_KP_ENTER):
		return VirtualKey::KEY_KP_ENTER;
	case (GLFW_KEY_KP_EQUAL):
		return VirtualKey::KEY_KP_EQUAL;
	case (GLFW_KEY_LEFT_SHIFT):
		return VirtualKey::KEY_LEFT_SHIFT;
	case (GLFW_KEY_LEFT_CONTROL):
		return VirtualKey::KEY_LEFT_CONTROL;
	case (GLFW_KEY_LEFT_ALT):
		return VirtualKey::KEY_LEFT_ALT;
	case (GLFW_KEY_LEFT_SUPER):
		return VirtualKey::KEY_LEFT_SUPER;
	case (GLFW_KEY_RIGHT_SHIFT):
		return VirtualKey::KEY_RIGHT_SHIFT;
	case (GLFW_KEY_RIGHT_CONTROL):
		return VirtualKey::KEY_RIGHT_CONTROL;
	case (GLFW_KEY_RIGHT_ALT):
		return VirtualKey::KEY_RIGHT_ALT;
	case (GLFW_KEY_RIGHT_SUPER):
		return VirtualKey::KEY_RIGHT_SUPER;
	case (GLFW_KEY_MENU):
		return VirtualKey::KEY_MENU;
	default:
		return VirtualKey::KEY_UNKNOWN;
	}
}

int CInputGLFW::GetModifiers() const
{
	int result = 0;
	if (m_modifiers & GLFW_MOD_ALT)
		result |= MODIFIER_ALT;
	if (m_modifiers & GLFW_MOD_SHIFT)
		result |= MODIFIER_SHIFT;
	if (m_modifiers & GLFW_MOD_CONTROL)
		result |= MODIFIER_CTRL;
	return result;
}

bool CInputGLFW::IsKeyPressed(VirtualKey key) const
{
	return glfwGetKey(m_window, static_cast<int>(key)) == GLFW_PRESS;
}
