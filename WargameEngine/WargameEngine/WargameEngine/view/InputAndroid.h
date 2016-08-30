#pragma once
#include "IInput.h"
#include "android\input.h"
#include "..\Signal.h"

class CInputAndroid : public IInput
{
public:
	void HandleInput(AInputEvent* event);

	virtual void DoOnLMBDown(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnLMBUp(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnRMBDown(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnRMBUp(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnMouseWheelUp(std::function<bool() > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnMouseWheelDown(std::function<bool() > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnKeyDown(std::function<bool(int key, int modifiers) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnKeyUp(std::function<bool(int key, int modifiers) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnCharacter(std::function<bool(unsigned int character) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnMouseMove(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnGamepadButtonStateChange(std::function<bool(int gamepadIndex, int buttonIndex, bool newState) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnGamepadAxisChange(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void EnableCursor(bool enable = true) override;
	virtual int GetModifiers() const override;
	virtual int GetMouseX() const override;
	virtual int GetMouseY() const override;
	virtual void DeleteAllSignalsByTag(std::string const& tag) override;
	virtual VirtualKey KeycodeToVirtualKey(int key) const override;
private:
	CSignal<int, int> m_onLMBDown;
	CSignal<int, int> m_onLMBUp;
	CSignal<int, int> m_onRMBDown;
	CSignal<int, int> m_onRMBUp;
	CSignal<> m_onZoomIn;
	CSignal<> m_onZoomOut;
	CSignal<int, int> m_onKeyDown;
	CSignal<int, int> m_onKeyUp;
	CSignal<unsigned int> m_onCharacter;
	CSignal<int, int> m_onMouseMove;
	int m_lastX, m_lastY;
};
