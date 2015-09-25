#pragma once
#include "IInput.h"
#include <memory>
#include <GLFW\glfw3.h>

class IObject;

class CInputGLFW : public IInput
{
public:
	CInputGLFW(GLFWwindow * window);
	virtual void DoOnLMBDown(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnLMBUp(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnRMBDown(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnRMBUp(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnMouseWheelUp(std::function<bool() > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnMouseWheelDown(std::function<bool() > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnKeyDown(std::function<bool(int key, int modifiers) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnKeyUp(std::function<bool(int key, int modifiers) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnCharacter(std::function<bool(unsigned int character) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void DoOnMouseMove(std::function<bool(int, int) > const& handler, int priority = 0, std::string const& tag = "") override;
	virtual void EnableCursor(bool enable = true) override;
	virtual int GetModifiers() const override;
	virtual int GetMouseX() const override;
	virtual int GetMouseY() const override;
	virtual void DeleteAllSignalsByTag(std::string const& tag) override;

	static void OnMouse(GLFWwindow* window, int button, int action, int mods);
	static void OnScroll(GLFWwindow* window, double xoffset, double yoffset);
	static void OnKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void OnCharacter(GLFWwindow* window, unsigned int key);
	static void OnMouseMove(GLFWwindow* window, double xpos, double ypos);
private:
	GLFWwindow * m_window;
	struct sSignals;
	static std::unique_ptr<sSignals> m_signals;
	static bool m_cursorEnabled;
};
