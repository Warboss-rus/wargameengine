#include "InputBase.h"

using namespace signals;

namespace wargameEngine
{
namespace view
{
SignalConnection InputBase::DoOnLMBDown(const MouseClickHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onLMBDown.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnLMBUp(const MouseClickHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onLMBUp.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnRMBDown(const MouseClickHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onRMBDown.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnRMBUp(const MouseClickHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onRMBUp.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnMouseWheel(const MouseWheelHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onWheel.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnKeyDown(const KeyboardHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onKeyDown.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnKeyUp(const KeyboardHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onKeyUp.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnCharacter(const CharacterHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onCharacter.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnMouseMove(const MouseMoveHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onMouseMove.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnGamepadButtonStateChange(const GamepadButtonHandler& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	return m_onGamepadButton.Connect(handler, priority, tag);
}

SignalConnection InputBase::DoOnGamepadAxisChange(const GamepadAxisHandler& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	return m_onGamepadAxis.Connect(handler, priority, tag);
}

const float* InputBase::GetHeadTrackingMatrix(size_t deviceIndex) const
{
	return m_headTrackings.at(deviceIndex);
}

CVector3f InputBase::GetHeadTrackingRotations(size_t deviceIndex) const
{
	return m_headRotations.at(deviceIndex);
}

void InputBase::DeleteAllSignalsByTag(std::string const& tag)
{
	m_onLMBDown.RemoveByTag(tag);
	m_onLMBUp.RemoveByTag(tag);
	m_onRMBDown.RemoveByTag(tag);
	m_onRMBUp.RemoveByTag(tag);
	m_onWheel.RemoveByTag(tag);
	m_onKeyDown.RemoveByTag(tag);
	m_onKeyUp.RemoveByTag(tag);
	m_onCharacter.RemoveByTag(tag);
	m_onMouseMove.RemoveByTag(tag);
	m_onGamepadButton.RemoveByTag(tag);
	m_onGamepadAxis.RemoveByTag(tag);
}


void InputBase::Reset()
{
	m_onLMBDown.Reset();
	m_onLMBUp.Reset();
	m_onRMBDown.Reset();
	m_onRMBUp.Reset();
	m_onWheel.Reset();
	m_onKeyDown.Reset();
	m_onKeyUp.Reset();
	m_onCharacter.Reset();
	m_onMouseMove.Reset();
	m_onGamepadButton.Reset();
	m_onGamepadAxis.Reset();
}


bool InputBase::IsLMBPressed() const
{
	return m_lmbDown;
}


bool InputBase::IsRMBPressed() const
{
	return m_rmbDown;
}

void InputBase::OnLMBDown(int x, int y)
{
	m_onLMBDown(x, y);
	m_prevX = x;
	m_prevY = y;
	m_lmbDown = true;
}

void InputBase::OnLMBUp(int x, int y)
{
	m_onLMBUp(x, y);
	m_prevX = x;
	m_prevY = y;
	m_lmbDown = false;
}

void InputBase::OnRMBDown(int x, int y)
{
	m_onRMBDown(x, y);
	m_prevX = x;
	m_prevY = y;
	m_rmbDown = true;
}

void InputBase::OnRMBUp(int x, int y)
{
	m_onRMBUp(x, y);
	m_prevX = x;
	m_prevY = y;
	m_rmbDown = false;
}

void InputBase::OnMouseWheel(float delta)
{
	m_onWheel(delta);
}

void InputBase::OnKeyDown(VirtualKey key, int nativeKey)
{
	m_onKeyDown(key, nativeKey);
}

void InputBase::OnKeyUp(VirtualKey key, int nativeKey)
{
	m_onKeyUp(key, nativeKey);
}

void InputBase::OnCharacter(wchar_t character)
{
	m_onCharacter(character);
}

void InputBase::OnMouseMove(int x, int y)
{
	m_onMouseMove(x, y, x - m_prevX, y - m_prevY);
	m_prevX = x;
	m_prevY = y;
}

void InputBase::OnGamepadButton(int gamepadIndex, int buttonIndex, bool newState)
{
	m_onGamepadButton(gamepadIndex, buttonIndex, newState);
}

void InputBase::OnGamepadAxis(int gamepadIndex, int axisIndex, double horizontal, double vertical)
{
	m_onGamepadAxis(gamepadIndex, axisIndex, horizontal, vertical);
}

void InputBase::OnHeadRotation(size_t deviceIndex, const float* data)
{
	if (deviceIndex + 1 > m_headTrackings.size())
	{
		m_headTrackings.resize(deviceIndex + 1);
	}
	m_headTrackings[deviceIndex] = data;
}

void InputBase::OnHeadRotation(size_t deviceIndex, const CVector3f& data)
{
	if (deviceIndex + 1 > m_headRotations.size())
	{
		m_headRotations.resize(deviceIndex + 1);
	}
	m_headRotations[deviceIndex] = data;
}

}
}