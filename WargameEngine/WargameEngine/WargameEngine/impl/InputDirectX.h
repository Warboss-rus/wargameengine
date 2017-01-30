#pragma once
#include "../view/InputBase.h"
#include <vector>
#include <Windows.h>
#include <Xinput.h>

class CInputDirectX : public CInputBase
{
public:
	CInputDirectX(HWND hWnd);
	virtual void EnableCursor(bool enable = true) override;
	virtual int GetModifiers() const override;
	virtual int GetMouseX() const override;
	virtual int GetMouseY() const override;
	virtual VirtualKey KeycodeToVirtualKey(int key) const override;

	bool ProcessEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void UpdateControllers();
private:
	HWND m_hWnd;
	bool m_cursorEnabled = true;
	std::vector<XINPUT_STATE> m_gamepadStates;
};