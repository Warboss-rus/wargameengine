#pragma once
#include "../Signal.h"
#include "IInput.h"

class CInputBase : public IInput
{
public:
	void DoOnLMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnLMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnRMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnRMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnMouseWheel(const MouseWheelHandler& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnKeyDown(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnKeyUp(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnCharacter(const CharacterHandler& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnMouseMove(const MouseMoveHandler& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnGamepadButtonStateChange(std::function<bool(int gamepadIndex, int buttonIndex, bool newState)> const& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnGamepadAxisChange(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)> const& handler, int priority = 0, std::string const& tag = "") override;
	void DoOnHeadRotationChange(std::function<bool(int deviceIndex, float x, float y, float z)> const& handler, int priority = 0, std::string const& tag = "") override;
	void DeleteAllSignalsByTag(std::string const& tag) override;
	void Reset() override;

protected:
	void OnLMBDown(int x, int y);
	void OnLMBUp(int x, int y);
	void OnRMBDown(int x, int y);
	void OnRMBUp(int x, int y);
	void OnMouseWheel(int delta);
	void OnKeyDown(VirtualKey key, int nativeKey);
	void OnKeyUp(VirtualKey key, int nativeKey);
	void OnCharacter(wchar_t character);
	void OnMouseMove(int x, int y);
	void OnGamepadButton(int gamepadIndex, int buttonIndex, bool newState);
	void OnGamepadAxis(int gamepadIndex, int axisIndex, double horizontal, double vertical);
	void OnHeadRotation(int deviceIndex, float x, float y, float z);

private:
	CExclusiveSignal<int, int> m_onLMBDown;
	CExclusiveSignal<int, int> m_onLMBUp;
	CExclusiveSignal<int, int> m_onRMBDown;
	CExclusiveSignal<int, int> m_onRMBUp;
	CExclusiveSignal<int> m_onWheel;
	CExclusiveSignal<VirtualKey, int> m_onKeyDown;
	CExclusiveSignal<VirtualKey, int> m_onKeyUp;
	CExclusiveSignal<wchar_t> m_onCharacter;
	CExclusiveSignal<int, int, int, int> m_onMouseMove;
	CExclusiveSignal<int, int, bool> m_onGamepadButton;
	CExclusiveSignal<int, int, double, double> m_onGamepadAxis;
	CExclusiveSignal<int, float, float, float> m_onHeadRotation;
	int m_prevX, m_prevY;
};