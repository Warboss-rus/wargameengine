#pragma once
#include "../view/InputBase.h"
#include <vector>
#include <Windows.h>
#include <Xinput.h>

class CInputDirectX : public wargameEngine::view::InputBase
{
public:
	CInputDirectX(HWND hWnd);
	void EnableCursor(bool enable = true) override;
	int GetModifiers() const override;
	int GetMouseX() const override;
	int GetMouseY() const override;
	bool IsKeyPressed(wargameEngine::view::VirtualKey key) const override;

	bool ProcessEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void UpdateControllers();
private:
	static wargameEngine::view::VirtualKey KeycodeToVirtualKey(int key);

	HWND m_hWnd;
	bool m_cursorEnabled = true;
	std::vector<XINPUT_STATE> m_gamepadStates;
};