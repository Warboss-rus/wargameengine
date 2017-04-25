#include "InputBase.h"

CSignalConnection CInputBase::DoOnLMBDown(const MouseClickHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onLMBDown.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnLMBUp(const MouseClickHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onLMBUp.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnRMBDown(const MouseClickHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onRMBDown.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnRMBUp(const MouseClickHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onRMBUp.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnMouseWheel(const MouseWheelHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onWheel.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnKeyDown(const KeyboardHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onKeyDown.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnKeyUp(const KeyboardHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onKeyUp.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnCharacter(const CharacterHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onCharacter.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnMouseMove(const MouseMoveHandler& handler, int priority /*= 0*/, std::string const& tag)
{
	return m_onMouseMove.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnGamepadButtonStateChange(std::function<bool(int gamepadIndex, int buttonIndex, bool newState)> const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	return m_onGamepadButton.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnGamepadAxisChange(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)> const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	return m_onGamepadAxis.Connect(handler, priority, tag);
}

CSignalConnection CInputBase::DoOnHeadRotationChange(std::function<bool(int deviceIndex, float x, float y, float z)> const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	return m_onHeadRotation.Connect(handler, priority, tag);
}

void CInputBase::DeleteAllSignalsByTag(std::string const& tag)
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


void CInputBase::Reset()
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


bool CInputBase::IsLMBPressed() const
{
	return m_lmbDown;
}


bool CInputBase::IsRMBPressed() const
{
	return m_rmbDown;
}

void CInputBase::OnLMBDown(int x, int y)
{
	m_onLMBDown(x, y);
	m_prevX = x;
	m_prevY = y;
	m_lmbDown = true;
}

void CInputBase::OnLMBUp(int x, int y)
{
	m_onLMBUp(x, y);
	m_prevX = x;
	m_prevY = y;
	m_lmbDown = false;
}

void CInputBase::OnRMBDown(int x, int y)
{
	m_onRMBDown(x, y);
	m_prevX = x;
	m_prevY = y;
	m_rmbDown = true;
}

void CInputBase::OnRMBUp(int x, int y)
{
	m_onRMBUp(x, y);
	m_prevX = x;
	m_prevY = y;
	m_rmbDown = false;
}

void CInputBase::OnMouseWheel(float delta)
{
	m_onWheel(delta);
}

void CInputBase::OnKeyDown(VirtualKey key, int nativeKey)
{
	m_onKeyDown(key, nativeKey);
}

void CInputBase::OnKeyUp(VirtualKey key, int nativeKey)
{
	m_onKeyUp(key, nativeKey);
}

void CInputBase::OnCharacter(wchar_t character)
{
	m_onCharacter(character);
}

void CInputBase::OnMouseMove(int x, int y)
{
	m_onMouseMove(x, y, x - m_prevX, y - m_prevY);
	m_prevX = x;
	m_prevY = y;
}

void CInputBase::OnGamepadButton(int gamepadIndex, int buttonIndex, bool newState)
{
	m_onGamepadButton(gamepadIndex, buttonIndex, newState);
}

void CInputBase::OnGamepadAxis(int gamepadIndex, int axisIndex, double horizontal, double vertical)
{
	m_onGamepadAxis(gamepadIndex, axisIndex, horizontal, vertical);
}

void CInputBase::OnHeadRotation(int deviceIndex, float x, float y, float z)
{
	m_onHeadRotation(deviceIndex, x, y, z);
}
