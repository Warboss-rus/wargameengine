#pragma once
#include "../view/InputBase.h"
#include <memory>
#include <vector>
#include <GLFW\glfw3.h>

class IObject;

class CInputGLFW : public CInputBase
{
public:
	CInputGLFW(GLFWwindow * window);

	virtual void EnableCursor(bool enable = true) override;
	virtual int GetMouseX() const override;
	virtual int GetMouseY() const override;
	virtual VirtualKey CInputGLFW::KeycodeToVirtualKey(int key) const override;
	virtual int CInputGLFW::GetModifiers() const override;

	static void OnMouse(GLFWwindow* window, int button, int action, int mods);
	static void OnScroll(GLFWwindow* window, double xoffset, double yoffset);
	static void OnKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void CharacterCallback(GLFWwindow* window, unsigned int key);
	static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
	void UpdateControllers();
	void SetHeadRotation(int deviceIndex, double x, double y, double z);
private:
	GLFWwindow * m_window;
	static CInputGLFW * m_instance;
	bool m_cursorEnabled = true;
	struct sControllerState
	{
		std::vector<unsigned char> buttons;
		std::vector<float> axes;
	};
	std::vector<sControllerState> m_gamepadStates;
	int m_modifiers = 0;
};
