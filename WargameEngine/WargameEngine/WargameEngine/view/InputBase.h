#pragma once
#include "../Signal.h"
#include "IInput.h"
#include "Matrix4.h"

namespace wargameEngine
{
namespace view
{
class InputBase : public IInput
{
public:
	signals::SignalConnection DoOnLMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnLMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnRMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnRMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnMouseWheel(const MouseWheelHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnKeyDown(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnKeyUp(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnCharacter(const CharacterHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnMouseMove(const MouseMoveHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnGamepadButtonStateChange(const GamepadButtonHandler& handler, int priority = 0, std::string const& tag = "") override;
	signals::SignalConnection DoOnGamepadAxisChange(const GamepadAxisHandler& handler, int priority = 0, std::string const& tag = "") override;
	const float* GetHeadTrackingMatrix(size_t deviceIndex) const override;
	CVector3f GetHeadTrackingRotations(size_t deviceIndex) const override;
	void DeleteAllSignalsByTag(std::string const& tag) override;
	void Reset() override;

	bool IsLMBPressed() const override;
	bool IsRMBPressed() const override;

	void OnHeadRotation(size_t deviceIndex, const float* data);
	void OnHeadRotation(size_t deviceIndex, const CVector3f& data);

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
	signals::ExclusiveSignal<int, int> m_onLMBDown;
	signals::ExclusiveSignal<int, int> m_onLMBUp;
	signals::ExclusiveSignal<int, int> m_onRMBDown;
	signals::ExclusiveSignal<int, int> m_onRMBUp;
	signals::ExclusiveSignal<float> m_onWheel;
	signals::ExclusiveSignal<VirtualKey, int> m_onKeyDown;
	signals::ExclusiveSignal<VirtualKey, int> m_onKeyUp;
	signals::ExclusiveSignal<wchar_t> m_onCharacter;
	signals::ExclusiveSignal<int, int, int, int> m_onMouseMove;
	signals::ExclusiveSignal<int, int, bool> m_onGamepadButton;
	signals::ExclusiveSignal<int, int, double, double> m_onGamepadAxis;
	std::vector<Matrix4F> m_headTrackings;
	std::vector<CVector3f> m_headRotations;
	int m_prevX = 0;
	int m_prevY = 0;
	bool m_lmbDown = false;
	bool m_rmbDown = false;
};
}
}