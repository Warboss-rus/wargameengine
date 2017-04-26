#pragma once
#include "KeyDefines.h"
#include <functional>
#include "..\Signal.h"

class IInput
{
public:
	using MouseClickHandler = std::function<bool(int x, int y)>;
	using MouseWheelHandler = std::function<bool(float delta)>;
	using MouseMoveHandler = std::function<bool(int newX, int newY, int deltaX, int deltaY)>;
	using KeyboardHandler = std::function<bool(VirtualKey virtualKey, int nativeKey)>;
	using CharacterHandler = std::function<bool(wchar_t character)>;
	using GamepadButtonHandler = std::function<bool(int gamepadIndex, int buttonIndex, bool newState)>;
	using GamepadAxisHandler = std::function<bool(int gamepadIndex, int axisIndex, double horizontal, double vertical)>;

	enum Modifiers
	{
		MODIFIER_SHIFT = 1,
		MODIFIER_CTRL = 2,
		MODIFIER_ALT = 4
	};

	virtual CSignalConnection DoOnLMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnLMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnRMBDown(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnRMBUp(const MouseClickHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnMouseWheel(const MouseWheelHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnKeyDown(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnKeyUp(const KeyboardHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnCharacter(const CharacterHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnMouseMove(const MouseMoveHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnGamepadButtonStateChange(const GamepadButtonHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual CSignalConnection DoOnGamepadAxisChange(const GamepadAxisHandler& handler, int priority = 0, std::string const& tag = "") = 0;
	virtual void EnableCursor(bool enable = true) = 0;
	virtual int GetMouseX() const = 0;
	virtual int GetMouseY() const = 0;
	virtual int GetModifiers() const = 0;
	virtual bool IsKeyPressed(VirtualKey key) const = 0;
	virtual bool IsLMBPressed() const = 0;
	virtual bool IsRMBPressed() const = 0;
	virtual void Reset() = 0;
	virtual void DeleteAllSignalsByTag(std::string const& tag) = 0;
	virtual const float* GetHeadTrackingMatrix(size_t deviceIndex) const = 0;

	virtual ~IInput() {}
};
