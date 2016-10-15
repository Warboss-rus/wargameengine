#pragma once
#include <memory>
#include "../view/InputBase.h"

class IObject;

class CInputGLUT : public CInputBase
{
public:
	CInputGLUT();
	virtual void EnableCursor(bool enable = true) override;
	virtual int GetModifiers() const override;
	virtual int GetMouseX() const override;
	virtual int GetMouseY() const override;
	virtual VirtualKey KeycodeToVirtualKey(int key) const override;

	static void OnSpecialKeyPress(int key, int x, int y);
	static void OnSpecialKeyRelease(int key, int x, int y);
	static void OnMouse(int button, int state, int x, int y);
	static void OnKeyboard(unsigned char key, int x, int y);
	static void OnKeyboardUp(unsigned char key, int x, int y);
	static void OnPassiveMouseMove(int x, int y);
	static void MouseMoveCallback(int x, int y);

	static void OnJoystick(unsigned int buttonMask, int x, int y, int z);
private:
	static CInputGLUT * m_instance;
	bool m_cursorEnabled = true;
	unsigned int m_joystickButtons = 0;
	int m_joystickAxes[3] = {0, 0, 0};
};
