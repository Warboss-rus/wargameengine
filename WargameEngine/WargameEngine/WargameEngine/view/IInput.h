#pragma once
#include "KeyDefines.h"
#include <functional>

class IInput
{
public:
	using MouseClickHandler = std::function<bool(int x, int y)>;
	using MouseWheelHandler = std::function<bool(int delta)>;
	using MouseMoveHandler = std::function<bool(int newX, int newY, int deltaX, int deltaY)>;
	using KeyboardHandler = std::function<bool(VirtualKey virtualKey, int nativeKey)>;
	using CharacterHandler = std::function<bool(wchar_t character)>;

	enum Modifiers
	{
		MODIFIER_SHIFT = 1,
		MODIFIER_CTRL = 2,
		MODIFIER_ALT = 4
	};

	virtual void DoOnLMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnLMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnRMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnRMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnMouseWheel(const MouseWheelHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnKeyDown(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnKeyUp(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnCharacter(const CharacterHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnMouseMove(const MouseMoveHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnGamepadButtonStateChange(std::function<bool(int gamepadIndex, int buttonIndex, bool newState)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnGamepadAxisChange(std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void DoOnHeadRotationChange(std::function<bool(int deviceIndex, float x, float y, float z)> const& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void EnableCursor(bool enable = true) = 0;
	virtual int GetMouseX() const = 0;
	virtual int GetMouseY() const = 0;
	virtual int GetModifiers() const = 0;
	virtual bool IsKeyPressed(VirtualKey key) const = 0;
	virtual void Reset() = 0;
	virtual void DeleteAllSignalsByTag(std::string const& tag) = 0;

	virtual ~IInput() {}
};
