#include "InputDirectX.h"
#include <stdexcept>
#include <Windowsx.h>
#include <map>
#include <intsafe.h>

CInputDirectX::CInputDirectX(HWND hWnd)
	:m_hWnd(hWnd), m_cursorEnabled(true)
{
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
		OnLMBDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}break;
	case WM_LBUTTONUP:
	{
		OnLMBUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}break;
	case WM_RBUTTONDOWN:
	{
		OnRMBDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}break;
	case WM_RBUTTONUP:
	{
		OnRMBUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}break;
	case WM_MOUSEWHEEL:
	{
		if ((HIWORD(wParam) & 0xf000) == 0)
		{
			OnMouseWheelUp();
		}
		else
		{
			OnMouseWheelDown();
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
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
		OnKeyDown(wParam, GetModifiers());
	}break;
	case WM_KEYUP:
	{
		OnKeyUp(wParam, GetModifiers());
	}break;
	case WM_CHAR:
	{
		OnCharacter(static_cast<wchar_t>(wParam));
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

void CInputDirectX::UpdateControllers()
{
	DWORD dwResult;
	XINPUT_STATE state;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		dwResult = XInputGetState(i, &state);

		if (dwResult == ERROR_SUCCESS)
		{
			if (m_gamepadStates.size() <= i)
			{
				XINPUT_STATE emptyState;
				ZeroMemory(&emptyState, sizeof(XINPUT_STATE));
				m_gamepadStates.resize(i + 1, emptyState);
			}
			auto& oldState = m_gamepadStates.at(i).Gamepad;
			for (WORD j = XINPUT_GAMEPAD_DPAD_UP, k = 0; k < 16; j *= 2, ++k)
			{
				if ((state.Gamepad.wButtons & j) != (oldState.wButtons & j))
				{
					OnGamepadButton(i, k, !!(state.Gamepad.wButtons & j));
				}
			}
			if ((abs(state.Gamepad.sThumbLX - oldState.sThumbLX) > 0) || (abs(state.Gamepad.sThumbLY - oldState.sThumbLY) > 0))
			{
				OnGamepadAxis(i, 0, state.Gamepad.sThumbLX / SHORT_MAX, state.Gamepad.sThumbLY / SHORT_MAX);
			}
			if ((abs(state.Gamepad.sThumbRX - oldState.sThumbRX) > 0) || (abs(state.Gamepad.sThumbRY - oldState.sThumbRY) > 0))
			{
				OnGamepadAxis(i, 1, state.Gamepad.sThumbLX / SHORT_MAX, state.Gamepad.sThumbLY / SHORT_MAX);
			}
			if (state.Gamepad.bLeftTrigger != oldState.bLeftTrigger)//left trigger
			{
				if ((state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) != (oldState.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
				{
					OnGamepadButton(i, 16, state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				}
				OnGamepadAxis(i, 2, static_cast<double>(state.Gamepad.bLeftTrigger - state.Gamepad.bRightTrigger) / BYTE_MAX, 0.0);
			}
			if (state.Gamepad.bRightTrigger != oldState.bRightTrigger)//right trigger
			{
				if ((state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) != (oldState.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
				{
					OnGamepadButton(i, 17, state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				}
				OnGamepadAxis(i, 2, static_cast<double>(state.Gamepad.bLeftTrigger - state.Gamepad.bRightTrigger) / BYTE_MAX, 0.0);
			}
			m_gamepadStates[i] = state;
		}
	}
}
