#include "InputGLUT.h"
#include <map>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#pragma warning(push)
#pragma warning(disable: 4505)
#include <GL/glut.h>
#pragma warning(pop)
#endif

using namespace wargameEngine;
using namespace view;

CInputGLUT* CInputGLUT::m_instance = nullptr;
static const int SCROLL_UP = 3;
static const int SCROLL_DOWN = 4;
static int g_prevX;
static int g_prevY;

static const std::map<int, IInput::Modifiers> modifiersMap = {
	{ GLUT_ACTIVE_ALT, IInput::MODIFIER_ALT },
	{ GLUT_ACTIVE_CTRL, IInput::MODIFIER_CTRL },
	{ GLUT_ACTIVE_SHIFT, IInput::MODIFIER_SHIFT },
};

int GetModifiers()
{
	int glut = glutGetModifiers();
	int result = 0;
	for (auto pair : modifiersMap)
	{
		if (glut & pair.first)
		{
			result |= pair.second;
		}
	}
	return result;
}

void CInputGLUT::OnMouse(int button, int state, int x, int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON: 
		if (state == GLUT_DOWN)
		{
			m_instance->OnLMBDown(x, y);
		}
		else
		{
			m_instance->OnLMBUp(x, y);
		}break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			m_instance->OnRMBDown(x, y);
		}
		else
		{
			m_instance->OnRMBUp(x, y);
		}break;
	case SCROLL_UP:
		if (state == GLUT_UP)
		{
			m_instance->OnMouseWheel(1.0f);
		}break;
	case SCROLL_DOWN:
		if (state == GLUT_UP)
		{
			m_instance->OnMouseWheel(-1.0f);
		}break;
	}
}

bool HasModifier(int modifier)
{
	return (glutGetModifiers() & modifier) != 0;
}

void CInputGLUT::OnKeyboard(unsigned char key, int /*x*/, int /*y*/)
{
	m_instance->OnKeyDown(KeycodeToVirtualKey(key), static_cast<int>(key));
}

void CInputGLUT::OnKeyboardUp(unsigned char key, int , int)
{
	m_instance->OnKeyUp(KeycodeToVirtualKey(key), key);
	if (key >= 32 && key != 127)
	{
		wchar_t str;
		mbstowcs(&str, reinterpret_cast<const char*>(&key), 1);
		m_instance->OnCharacter(str);
	}
}

CInputGLUT::CInputGLUT()
{
	m_instance = this;
}

static int screenCenterX = 320;
static int screenCenterY = 240;
static int g_lastMouseX = screenCenterX;
static int g_lastMouseY = screenCenterY;

void CInputGLUT::EnableCursor(bool enable /*= true*/)
{
	if (enable)
	{
		glutWarpPointer(g_prevX, g_prevY);
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
	else
	{
		glutSetCursor(GLUT_CURSOR_NONE);
		g_prevX = g_lastMouseX;
		g_prevY = g_lastMouseY;
		screenCenterX = glutGet(GLUT_WINDOW_WIDTH) / 2;
		screenCenterY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
		glutWarpPointer(screenCenterX, screenCenterY);
		g_lastMouseX = screenCenterX;
		g_lastMouseY = screenCenterY;
	}
	m_cursorEnabled = enable;
}

void CInputGLUT::OnSpecialKeyPress(int key, int /*x*/, int /*y*/)
{
	m_instance->OnKeyDown(KeycodeToVirtualKey(key), key);
}

void CInputGLUT::OnSpecialKeyRelease(int key, int, int)
{
	m_instance->OnKeyUp(KeycodeToVirtualKey(key), key);
}

void CInputGLUT::OnPassiveMouseMove(int x, int y)
{
	MouseMoveCallback(x, y);
}

void CInputGLUT::MouseMoveCallback(int x, int y)
{
	static bool just_warped = false;
	if (just_warped)
	{
		just_warped = false;
		return;
	}
	m_instance->OnMouseMove(x, y);
	if (!m_instance->m_cursorEnabled)
	{
		if (g_prevX == 0 && g_prevY)
		{
			g_prevX = x;
			g_prevY = y;
		}
		glutWarpPointer(screenCenterX, screenCenterY);
		just_warped = true;
	}
	else
	{
		g_lastMouseX = x;
		g_lastMouseY = y;
	}
}

void CInputGLUT::OnJoystick(unsigned int buttonMask, int x, int y, int z)
{
	for (unsigned int i = 0, k = 1; i < 32; ++i, k *= 2)
	{
		if ((buttonMask & k) != (m_instance->m_joystickButtons & k))
		{
			m_instance->OnGamepadButton(0, i, !!(buttonMask & k));
		}
	}
	m_instance->m_joystickButtons = buttonMask;
	if (x != m_instance->m_joystickAxes[0] || y != m_instance->m_joystickAxes[1])
	{
		m_instance->OnGamepadAxis(0, 0, x / 1000, y / 1000);
	}
	if (z != m_instance->m_joystickAxes[2])
	{
		m_instance->OnGamepadAxis(0, 1, z / 1000, 0);
	}
	m_instance->m_joystickAxes[0] = x;
	m_instance->m_joystickAxes[1] = y;
	m_instance->m_joystickAxes[2] = z;
}

int CInputGLUT::GetMouseX() const
{
	return g_lastMouseX;
}

int CInputGLUT::GetMouseY() const
{
	return g_lastMouseY;
}

VirtualKey CInputGLUT::KeycodeToVirtualKey(int key)
{
	static const std::map<int, VirtualKey> virtualKeys = {
		{8, VirtualKey::KEY_BACKSPACE},
		{GLUT_KEY_LEFT, VirtualKey::KEY_LEFT },
		{GLUT_KEY_UP, VirtualKey::KEY_UP },
		{GLUT_KEY_RIGHT, VirtualKey::KEY_RIGHT },
		{GLUT_KEY_DOWN, VirtualKey::KEY_DOWN },
		{GLUT_KEY_HOME, VirtualKey::KEY_HOME },
		{GLUT_KEY_END, VirtualKey::KEY_END },
		{127, VirtualKey::KEY_DELETE },
	};
	auto it = virtualKeys.find(key);
	return it == virtualKeys.end() ? VirtualKey::KEY_UNKNOWN : it->second;
}

int CInputGLUT::GetModifiers() const
{
	return ::GetModifiers();
}

bool CInputGLUT::IsKeyPressed(VirtualKey key) const
{
	return m_pressedKeys.find(key) != m_pressedKeys.end();
}
