#pragma once
#include "../Signal.h"
#include "IInput.h"
#include "Matrix4.h"

class CInputBase : public IInput
{
public:
	CSignalConnection DoOnLMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnLMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnRMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnRMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnMouseWheel(const MouseWheelHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnKeyDown(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnKeyUp(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnCharacter(const CharacterHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnMouseMove(const MouseMoveHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnGamepadButtonStateChange(const GamepadButtonHandler& handler, int priority = 0, std::string const& tag = "") override;
	CSignalConnection DoOnGamepadAxisChange(const GamepadAxisHandler& handler, int priority = 0, std::string const& tag = "") override;
	const float* GetHeadTrackingMatrix(size_t deviceIndex) const override;
	void DeleteAllSignalsByTag(std::string const& tag) override;
	void Reset() override;

	bool IsLMBPressed() const override;
	bool IsRMBPressed() const override;

	void OnHeadRotation(size_t deviceIndex, const float* data);

protected:
	void OnLMBDown(int x, int y);
	void OnLMBUp(int x, int y);
	void OnRMBDown(int x, int y);
	void OnRMBUp(int x, int y);
	void OnMouseWheel(float delta);
	void OnKeyDown(VirtualKey key, int nativeKey);
	void OnKeyUp(VirtualKey key, int nativeKey);
	void OnCharacter(wchar_t character);
	void OnMouseMove(int x, int y);
	void OnGamepadButton(int gamepadIndex, int buttonIndex, bool newState);
	void OnGamepadAxis(int gamepadIndex, int axisIndex, double horizontal, double vertical);

private:
	CExclusiveSignal<int, int> m_onLMBDown;
	CExclusiveSignal<int, int> m_onLMBUp;
	CExclusiveSignal<int, int> m_onRMBDown;
	CExclusiveSignal<int, int> m_onRMBUp;
	CExclusiveSignal<float> m_onWheel;
	CExclusiveSignal<VirtualKey, int> m_onKeyDown;
	CExclusiveSignal<VirtualKey, int> m_onKeyUp;
	CExclusiveSignal<wchar_t> m_onCharacter;
	CExclusiveSignal<int, int, int, int> m_onMouseMove;
	CExclusiveSignal<int, int, bool> m_onGamepadButton;
	CExclusiveSignal<int, int, double, double> m_onGamepadAxis;
	std::vector<Matrix4F> m_headTrackings;
	int m_prevX = 0;
	int m_prevY = 0;
	bool m_lmbDown = false;
	bool m_rmbDown = false;
};