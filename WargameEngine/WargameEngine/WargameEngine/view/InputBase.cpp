#include "InputBase.h"

void CInputBase::DoOnLMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onLMBDown.Connect(handler, priority, tag);
}

void CInputBase::DoOnLMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onLMBUp.Connect(handler, priority, tag);
}

void CInputBase::DoOnRMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onRMBDown.Connect(handler, priority, tag);
}

void CInputBase::DoOnRMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onRMBUp.Connect(handler, priority, tag);
}

void CInputBase::DoOnMouseWheelUp(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onWheelUp.Connect(handler, priority, tag);
}

void CInputBase::DoOnMouseWheelDown(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onWheelDown.Connect(handler, priority, tag);
}

void CInputBase::DoOnKeyDown(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onKeyDown.Connect(handler, priority, tag);
}

void CInputBase::DoOnKeyUp(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onKeyUp.Connect(handler, priority, tag);
}

void CInputBase::DoOnCharacter(std::function<bool(wchar_t character) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onCharacter.Connect(handler, priority, tag);
}

void CInputBase::DoOnMouseMove(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag)
{
	m_onMouseMove.Connect(handler, priority, tag);
}

void CInputBase::DoOnGamepadButtonStateChange(std::function<bool(int gamepadIndex, int buttonIndex, bool newState)> const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onGamepadButton.Connect(handler, priority, tag);
}

void CInputBase::DoOnGamepadAxisChange(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)> const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onGamepadAxis.Connect(handler, priority, tag);
}

void CInputBase::DoOnHeadRotationChange(std::function<bool(int deviceIndex, float x, float y, float z)> const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onHeadRotation.Connect(handler, priority, tag);
}

void CInputBase::DeleteAllSignalsByTag(std::string const& tag)
{
	m_onLMBDown.RemoveByTag(tag);
	m_onLMBUp.RemoveByTag(tag);
	m_onRMBDown.RemoveByTag(tag);
	m_onRMBUp.RemoveByTag(tag);
	m_onWheelUp.RemoveByTag(tag);
	m_onWheelDown.RemoveByTag(tag);
	m_onKeyDown.RemoveByTag(tag);
	m_onKeyUp.RemoveByTag(tag);
	m_onCharacter.RemoveByTag(tag);
	m_onMouseMove.RemoveByTag(tag);
	m_onGamepadButton.RemoveByTag(tag);
	m_onGamepadAxis.RemoveByTag(tag);
}

void CInputBase::OnLMBDown(int x, int y)
{
	m_onLMBDown(x, y);
}

void CInputBase::OnLMBUp(int x, int y)
{
	m_onLMBUp(x, y);
}

void CInputBase::OnRMBDown(int x, int y)
{
	m_onRMBDown(x, y);
}

void CInputBase::OnRMBUp(int x, int y)
{
	m_onRMBUp(x, y);
}

void CInputBase::OnMouseWheelDown()
{
	m_onWheelDown();
}

void CInputBase::OnMouseWheelUp()
{
	m_onWheelUp();
}

void CInputBase::OnKeyDown(int key, int mods)
{
	m_onKeyDown(key, mods);
}

void CInputBase::OnKeyUp(int key, int mods)
{
	m_onKeyUp(key, mods);
}

void CInputBase::OnCharacter(wchar_t character)
{
	m_onCharacter(character);
}

void CInputBase::OnMouseMove(int x, int y)
{
	m_onMouseMove(x, y);
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
