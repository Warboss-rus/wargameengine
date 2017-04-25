#pragma once
#include "android\input.h"
#include "../view/InputBase.h"

class CInputAndroid : public CInputBase
{
public:
	void HandleInput(AInputEvent* event);
	void HandleMotionEvent(int action, float x, float y);

	void EnableCursor(bool enable = true) override;
	int GetModifiers() const override;
	int GetMouseX() const override;
	int GetMouseY() const override;
	bool IsKeyPressed(VirtualKey key) const override;

private:
	static VirtualKey KeycodeToVirtualKey(int key);

	int m_lastX, m_lastY;
};
