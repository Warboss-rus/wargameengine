#pragma once
#include <memory>
#include <set>
#include "../view/InputBase.h"

class CInputGLUT : public wargameEngine::view::CInputBase
{
public:
	CInputGLUT();
	void EnableCursor(bool enable = true) override;
	int GetMouseX() const override;
	int GetMouseY() const override;
	int GetModifiers() const override;
	bool IsKeyPressed(wargameEngine::view::VirtualKey key) const override;

	static void OnSpecialKeyPress(int key, int x, int y);
	static void OnSpecialKeyRelease(int key, int x, int y);
	static void OnMouse(int button, int state, int x, int y);
	static void OnKeyboard(unsigned char key, int x, int y);
	static void OnKeyboardUp(unsigned char key, int x, int y);
	static void OnPassiveMouseMove(int x, int y);
	static void MouseMoveCallback(int x, int y);
	static void OnJoystick(unsigned int buttonMask, int x, int y, int z);
private:
	static wargameEngine::view::VirtualKey KeycodeToVirtualKey(int key);
	static CInputGLUT * m_instance;
	bool m_cursorEnabled = true;
	unsigned int m_joystickButtons = 0;
	int m_joystickAxes[3] = {0, 0, 0};
	std::set<wargameEngine::view::VirtualKey> m_pressedKeys;
};
