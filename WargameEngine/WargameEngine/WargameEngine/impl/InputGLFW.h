#pragma once
#include "../view/InputBase.h"
#include <memory>
#include <vector>
#include <GLFW/glfw3.h>

class IObject;

class CInputGLFW : public CInputBase
{
public:
	CInputGLFW(GLFWwindow * window);

	virtual void EnableCursor(bool enable = true) override;
	virtual int GetMouseX() const override;
	virtual int GetMouseY() const override;
	virtual VirtualKey KeycodeToVirtualKey(int key) const override;
	virtual int GetModifiers() const override;

	static void OnMouse(GLFWwindow* window, int button, int action, int mods);
	static void OnScroll(GLFWwindow* window, double xoffset, double yoffset);
	static void OnKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void CharacterCallback(GLFWwindow* window, unsigned int key);
	static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
	static void JoystickCallback(int joy, int event);
	void UpdateControllers();
	void SetHeadRotation(int deviceIndex, float x, float y, float z);
private:
	GLFWwindow * m_window;

	bool m_cursorEnabled = true;
	struct sControllerState
	{
		std::vector<unsigned char> buttons;
		std::vector<float> axes;
	};
	std::vector<sControllerState> m_gamepadStates;
	int m_modifiers = 0;
	std::vector<int> m_activeJoysticks;
	int m_prevX;
	int m_prevY;
};
