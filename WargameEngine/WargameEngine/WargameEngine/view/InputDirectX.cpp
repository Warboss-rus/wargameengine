#include "InputDirectX.h"
#include <stdexcept>
#include <Windowsx.h>

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
	if (enable)
	{
		SetCapture(m_hWnd);
	}
	else
	{
		ReleaseCapture();
	}
}

int CInputDirectX::GetModifiers() const
{
	int result = 0;
	if (GetKeyState(VK_CONTROL)) result |= MODIFIER_CTRL;
	if (GetKeyState(VK_SHIFT)) result |= MODIFIER_SHIFT;
	if (GetKeyState(VK_MENU)) result |= MODIFIER_ALT;
	return 0;
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

void CInputDirectX::ProcessEvent(UINT message, WPARAM wParam, LPARAM lParam)
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
		if (HIWORD(wParam) > 0)
		{
			m_onWheelUp();
		}
		else
		{
			m_onWheelUp();
		}
	}break;
	case WM_MOUSEMOVE:
	{
		m_onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	case WM_KEYDOWN:
	{
		m_onKeyDown(wParam, GetModifiers());
	}
	case WM_KEYUP:
	{
		m_onKeyUp(wParam, GetModifiers());
	}
	case WM_CHAR:
	{
		m_onCharacter(wParam);
	}
	}
}
