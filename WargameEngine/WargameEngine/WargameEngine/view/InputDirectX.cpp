#include "InputDirectX.h"
#include <stdexcept>
#include <Windowsx.h>
#include <map>

CInputDirectX::CInputDirectX(HWND hWnd)
	:m_hWnd(hWnd)
{
}

void CInputDirectX::DoOnLMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onLMBDown.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnLMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onLMBUp.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnRMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onRMBDown.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnRMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onRMBUp.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnMouseWheelUp(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onWheelUp.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnMouseWheelDown(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onWheelDown.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnKeyDown(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onKeyDown.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnKeyUp(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onKeyUp.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnCharacter(std::function<bool(unsigned int character) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onCharacter.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnMouseMove(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onMouseMove.Connect(handler, priority, tag);
}

void CInputDirectX::EnableCursor(bool enable /*= true*/)
{
	if (!enable)
	{
		SetCapture(m_hWnd);
		RECT screenSize;
		GetClientRect(m_hWnd, &screenSize);
		POINT center = { (screenSize.right - screenSize.left) / 2, (screenSize.bottom - screenSize.top) / 2 };
		ClientToScreen(m_hWnd, &center);
		SetCursorPos(center.x, center.y);
	}
	else
	{
		ReleaseCapture();
	}
	m_cursorEnabled = enable;
}

int CInputDirectX::GetModifiers() const
{
	int result = 0;
	if (GetKeyState(VK_CONTROL) & 0x8000) result |= MODIFIER_CTRL;
	if (GetKeyState(VK_SHIFT) & 0x8000) result |= MODIFIER_SHIFT;
	if (GetKeyState(VK_MENU) & 0x8000) result |= MODIFIER_ALT;
	return result;
}

int CInputDirectX::GetMouseX() const
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(m_hWnd, &point);
	return point.x;
}

int CInputDirectX::GetMouseY() const
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(m_hWnd, &point);
	return point.y;
}

void CInputDirectX::DeleteAllSignalsByTag(std::string const& tag)
{
	m_onLMBDown.RemoveByTag(tag);
	m_onLMBUp.RemoveByTag(tag);
	m_onRMBDown.RemoveByTag(tag);
	m_onRMBUp.RemoveByTag(tag);
	m_onWheelUp.RemoveByTag(tag);
	m_onWheelDown.RemoveByTag(tag);
	m_onKeyDown.RemoveByTag(tag);
	m_onKeyUp.RemoveByTag(tag);
	m_onCharacter.RemoveByTag(tag);
	m_onMouseMove.RemoveByTag(tag);
}

VirtualKey CInputDirectX::KeycodeToVirtualKey(int key) const
{
	static const std::map<int, VirtualKey> virtualKeys = {
		{ VK_BACK, KEY_BACKSPACE },
		{ VK_LEFT, KEY_LEFT },
		{ VK_UP, KEY_UP },
		{ VK_RIGHT, KEY_RIGHT },
		{ VK_DOWN, KEY_DOWN },
		{ VK_HOME, KEY_HOME },
		{ VK_END, KEY_END },
		{ VK_DELETE, KEY_DELETE },
	};
	auto it = virtualKeys.find(key);
	return it == virtualKeys.end() ? KEY_UNKNOWN : it->second;
}

bool CInputDirectX::ProcessEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		m_onLMBDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}break;
	case WM_LBUTTONUP:
	{
		m_onLMBUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}break;
	case WM_RBUTTONDOWN:
	{
		m_onRMBDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}break;
	case WM_RBUTTONUP:
	{
		m_onRMBUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}break;
	case WM_MOUSEWHEEL:
	{
		if ((HIWORD(wParam) & 0xf000) == 0)
		{
			m_onWheelUp();
		}
		else
		{
			m_onWheelDown();
		}
	}break;
	case WM_MOUSEMOVE:
	{
		static bool justWarped;
		if (justWarped)
		{
			justWarped = false;
			return true;
		}
		justWarped = false;
		m_onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (!m_cursorEnabled)
		{
			RECT screenSize;
			GetClientRect(m_hWnd, &screenSize);
			POINT center = { (screenSize.right - screenSize.left) / 2, (screenSize.bottom - screenSize.top) / 2 };
			ClientToScreen(m_hWnd, &center);
			SetCursorPos(center.x, center.y);
			justWarped = true;
		}
	}break;
	case WM_KEYDOWN:
	{
		m_onKeyDown(wParam, GetModifiers());
	}break;
	case WM_KEYUP:
	{
		m_onKeyUp(wParam, GetModifiers());
	}break;
	case WM_CHAR:
	{
		m_onCharacter(wParam);
	}break;
	case WM_SETCURSOR:
	{
		if (!m_cursorEnabled)
		{
			SetCursor(NULL);
		}
		else
		{
			return false;
		}
	}break;
	default:
		return false;
	}
	return true;
}
