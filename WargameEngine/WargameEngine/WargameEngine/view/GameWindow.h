#pragma once
#include <functional>
#include <memory>
#include <GLFW\glfw3.h>
#include "Input.h"

class CGameWindow
{
public:
	void Init();

	void Clear();
	void DoOnDrawScene(std::function<void()> const& handler);
	void DoOnResize(std::function<void(int, int)> const& handler);
	void DoOnShutdown(std::function<void()> const& handler);
	void ResizeWindow(int width, int height);
	void SetTitle(std::string const& title);
	void ToggleFullscreen();
	void Enter2DMode();
	void Leave2DMode();
	IInput& GetInput();
	void ResetInput();
private:
	void CreateNewWindow(GLFWmonitor * monitor = NULL);

	static void OnChangeState(GLFWwindow * window, int state);
	static void OnReshape(GLFWwindow * window, int width, int height);
	static void OnShutdown(GLFWwindow * window);

	GLFWwindow * m_window = NULL;
	std::unique_ptr<CInput>m_input;

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;

	bool m_2dMode;

	static bool m_visible;
};