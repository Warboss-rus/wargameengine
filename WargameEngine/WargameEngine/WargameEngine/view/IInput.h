#pragma once
#include <functional>
#include "KeyDefines.h"

class IInput
{
public:
	enum Modifiers
	{
		MODIFIER_SHIFT = 1,
		MODIFIER_CTRL = 2,
		MODIFIER_ALT = 4
	};
	virtual void DoOnLMBDown(std::function<bool(int, int)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnLMBUp(std::function<bool(int, int)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnRMBDown(std::function<bool(int, int)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnRMBUp(std::function<bool(int, int)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnMouseWheelUp(std::function<bool()> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnMouseWheelDown(std::function<bool()> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnKeyDown(std::function<bool(int key, int modifiers)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnKeyUp(std::function<bool(int key, int modifiers)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnCharacter(std::function<bool(wchar_t character)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnMouseMove(std::function<bool(int, int)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnGamepadButtonStateChange(std::function<bool(int gamepadIndex, int buttonIndex, bool newState)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnGamepadAxisChange(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnHeadRotationChange(std::function<bool(int deviceIndex, float x, float y, float z)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void EnableCursor(bool enable = true) = 0;
	virtual int GetModifiers() const = 0;
	virtual int GetMouseX() const = 0;
	virtual int GetMouseY() const = 0;
	virtual void DeleteAllSignalsByTag(std::string const& tag) = 0;
	virtual VirtualKey KeycodeToVirtualKey(int key) const = 0;

	virtual ~IInput() {}
};
