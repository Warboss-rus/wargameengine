#pragma once
#include <functional>
#include <memory>
#include "IGameWindow.h"
#include <GLFW\glfw3.h>
#include "IInput.h"

class COpenGLRenderer;

class CGameWindowGLFW : public IGameWindow
{
public:
	CGameWindowGLFW();
	virtual void LaunchMainLoop() override;
	virtual void DoOnDrawScene(std::function<void()> const& handler) override;
	virtual void DoOnResize(std::function<void(int, int)> const& handler) override;
	virtual void DoOnShutdown(std::function<void()> const& handler) override;
	virtual void ResizeWindow(int width, int height) override;
	virtual void SetTitle(std::string const& title) override;
	virtual void ToggleFullscreen() override;
	virtual void EnableMultisampling(bool enable, int level = 1.0f) override;
	virtual IInput& ResetInput() override;
	virtual IRenderer& GetRenderer() override;
	virtual IViewHelper& GetViewHelper() override;
private:
	void CreateNewWindow(GLFWmonitor * monitor = NULL);

	static void OnChangeState(GLFWwindow * window, int state);
	static void OnReshape(GLFWwindow * window, int width, int height);
	static void OnShutdown(GLFWwindow * window);

	GLFWwindow * m_window = NULL;
	std::unique_ptr<IInput>m_input;
	std::unique_ptr<COpenGLRenderer> m_renderer;

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;

	static bool m_visible;
};