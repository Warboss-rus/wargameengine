#pragma once
#include "android\input.h"
#include "../view/InputBase.h"

class CInputAndroid : public CInputBase
{
public:
	void HandleInput(AInputEvent* event);

	virtual void EnableCursor(bool enable = true) override;
	virtual int GetModifiers() const override;
	virtual int GetMouseX() const override;
	virtual int GetMouseY() const override;
	virtual VirtualKey KeycodeToVirtualKey(int key) const override;
private:
	int m_lastX, m_lastY;
};
