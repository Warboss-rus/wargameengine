#include "InputDirectX.h"
#include <stdexcept>

CInputDirectX::CInputDirectX(HWND hWnd)
	:m_hWnd(hWnd)
{
}

void CInputDirectX::DoOnLMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onLMBDown.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnLMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onLMBUp.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnRMBDown(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onRMBDown.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnRMBUp(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onRMBUp.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnMouseWheelUp(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onWheelUp.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnMouseWheelDown(std::function<bool() > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onWheelDown.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnKeyDown(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onKeyDown.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnKeyUp(std::function<bool(int key, int modifiers) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onKeyUp.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnCharacter(std::function<bool(unsigned int character) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onCharacter.Connect(handler, priority, tag);
}

void CInputDirectX::DoOnMouseMove(std::function<bool(int, int) > const& handler, int priority /*= 0*/, std::string const& tag /*= ""*/)
{
	m_onMouseMove.Connect(handler, priority, tag);
}

void CInputDirectX::EnableCursor(bool enable /*= true*/)
{
}

int CInputDirectX::GetModifiers() const
{
	return 0;
}

int CInputDirectX::GetMouseX() const
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(m_hWnd, &point);
	return point.x;
}

int CInputDirectX::GetMouseY() const
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(m_hWnd, &point);
	return point.y;
}

void CInputDirectX::DeleteAllSignalsByTag(std::string const& tag)
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
}
