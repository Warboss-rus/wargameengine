#pragma once
#include <functional>
#include <memory>
#include "IGameWindow.h"
#include <GLFW\glfw3.h>
#include "IInput.h"

class CGameWindowGLFW : public IGameWindow
{
public:
	virtual void Init() override;
	virtual void Clear() override;
	virtual void DoOnDrawScene(std::function<void()> const& handler) override;
	virtual void DoOnResize(std::function<void(int, int)> const& handler) override;
	virtual void DoOnShutdown(std::function<void()> const& handler) override;
	virtual void ResizeWindow(int width, int height) override;
	virtual void SetTitle(std::string const& title) override;
	virtual void ToggleFullscreen() override;
	virtual void Enter2DMode() override;
	virtual void Leave2DMode() override;
	virtual IInput& GetInput() override;
	virtual void ResetInput() override;
private:
	void CreateNewWindow(GLFWmonitor * monitor = NULL);

	static void OnChangeState(GLFWwindow * window, int state);
	static void OnReshape(GLFWwindow * window, int width, int height);
	static void OnShutdown(GLFWwindow * window);

	GLFWwindow * m_window = NULL;
	std::unique_ptr<IInput>m_input;

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;

	bool m_2dMode;

	static bool m_visible;
};