#pragma once
#include "IInput.h"
#include "..\Signal.h"

class CInputBase : public IInput
{
public:
	virtual void DoOnLMBDown(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnLMBUp(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnRMBDown(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnRMBUp(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnMouseWheelUp(std::function<bool() > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnMouseWheelDown(std::function<bool() > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnKeyDown(std::function<bool(int key, int modifiers) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnKeyUp(std::function<bool(int key, int modifiers) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnCharacter(std::function<bool(wchar_t character) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnMouseMove(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnGamepadButtonStateChange(std::function<bool(int gamepadIndex, int buttonIndex, bool newState)> const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnGamepadAxisChange(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)> const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DeleteAllSignalsByTag(std::string const& tag) override;
protected:
	void OnLMBDown(int x, int y);
	void OnLMBUp(int x, int y);
	void OnRMBDown(int x, int y);
	void OnRMBUp(int x, int y);
	void OnMouseWheelDown();
	void OnMouseWheelUp();
	void OnKeyDown(int key, int mods);
	void OnKeyUp(int key, int mods);
	void OnCharacter(wchar_t character);
	void OnMouseMove(int x, int y);
	void OnGamepadButton(int gamepadIndex, int buttonIndex, bool newState);
	void OnGamepadAxis(int gamepadIndex, int axisIndex, double horizontal, double vertical);
private:
	CSignal<int, int> m_onLMBDown;
	CSignal<int, int> m_onLMBUp;
	CSignal<int, int> m_onRMBDown;
	CSignal<int, int> m_onRMBUp;
	CSignal<> m_onWheelUp;
	CSignal<> m_onWheelDown;
	CSignal<int, int> m_onKeyDown;
	CSignal<int, int> m_onKeyUp;
	CSignal<wchar_t> m_onCharacter;
	CSignal<int, int> m_onMouseMove;
	CSignal<int, int, bool> m_onGamepadButton;
	CSignal<int, int, double, double> m_onGamepadAxis;
};