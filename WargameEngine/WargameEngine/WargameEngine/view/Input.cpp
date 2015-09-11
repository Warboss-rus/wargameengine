#include "Input.h"
#include <map>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "..\Signal.h"

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

void CInput::OnMouse(int button, int state, int x, int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON: 
		if (state == GLUT_DOWN)
		{
			m_signals->m_onLMBDown(x, y);
		}
		else
		{
			m_signals->m_onLMBUp(x, y);
		}break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			m_signals->m_onRMBDown(x, y);
		}
		else
		{
			m_signals->m_onRMBUp(x, y);
		}break;
	case SCROLL_UP:
		if (state == GLUT_UP)
		{
			m_signals->m_onWheelUp();
		}break;
	case SCROLL_DOWN:
		if (state == GLUT_UP)
		{
			m_signals->m_onWheelDown();
		}break;
	}
}

bool HasModifier(int modifier)
{
	return (glutGetModifiers() & modifier) != 0;
}

void CInput::OnKeyboard(unsigned char key, int /*x*/, int /*y*/)
{
	m_signals->m_onKeyDown(key, ::GetModifiers());
}

void CInput::OnKeyboardUp(unsigned char key, int , int)
{
	m_signals->m_onKeyUp(key, ::GetModifiers());
	if (key >= 32 && key != 127)
	{
		m_signals->m_onCharacter(key);
	}
}

int SpecialToKeyCode(int special)
{
	if(special >= GLUT_KEY_F1 && special <= GLUT_KEY_F12)
	{
		return 112 + special - GLUT_KEY_F1;
	}
	if(special >= GLUT_KEY_LEFT && special <= GLUT_KEY_DOWN)
	{
		return 37 + special - GLUT_KEY_LEFT;
	}
	if(special == GLUT_KEY_PAGE_UP || special == GLUT_KEY_PAGE_DOWN)
	{
		return 33 + special - GLUT_KEY_PAGE_UP;
	}
	if(special == GLUT_KEY_END)
	{
		return 35;
	}
	if (special == GLUT_KEY_HOME)
	{
		return 36;
	}
	if (special == GLUT_KEY_INSERT)
	{
		return 45;
	}
	return 0;
}

CInput::CInput()
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

void CInput::OnSpecialKeyPress(int key, int /*x*/, int /*y*/)
{
	int keycode = SpecialToKeyCode(key);
	m_signals->m_onKeyDown(keycode, ::GetModifiers());
}

void CInput::OnSpecialKeyRelease(int key, int, int)
{
	int keycode = SpecialToKeyCode(key);
	m_signals->m_onKeyUp(keycode, ::GetModifiers());
}

void CInput::OnPassiveMouseMove(int x, int y)
{
	OnMouseMove(x, y);
}

void CInput::OnMouseMove(int x, int y)
{
	static bool just_warped = false;
	if (just_warped)
	{
		just_warped = false;
		return;
	}
	m_signals->m_onMouseMove(x, y);
	if (!m_cursorEnabled)
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
	return ::GetModifiers();
}
