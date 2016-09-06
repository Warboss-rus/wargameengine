#pragma once
#include <functional>
#include <memory>
#include "IGameWindow.h"

class COpenGLRenderer;
class CInputGLFW;
struct GLFWwindow;
struct GLFWmonitor;

namespace vr
{
	class IVRSystem;
}

class CGameWindowVR : public IGameWindow
{
public:
	CGameWindowVR();
	~CGameWindowVR();
	virtual void LaunchMainLoop() override;
	virtual void DoOnDrawScene(std::function<void()> const& handler) override;
	virtual void DoOnResize(std::function<void(int, int)> const& handler) override;
	virtual void DoOnShutdown(std::function<void()> const& handler) override;
	virtual void ResizeWindow(int width, int height) override;
	virtual void SetTitle(std::wstring const& title) override;
	virtual void ToggleFullscreen() override;
	virtual void EnableMultisampling(bool enable, int level = 1.0f) override;
	virtual void EnableVRMode(bool enable) override;
	virtual IInput& ResetInput() override;
	virtual IRenderer& GetRenderer() override;
	virtual IViewHelper& GetViewHelper() override;
private:
	void CreateNewWindow(GLFWmonitor * monitor = NULL);

	static void OnChangeState(GLFWwindow * window, int state);
	static void OnReshape(GLFWwindow * window, int width, int height);
	static void OnShutdown(GLFWwindow * window);

	GLFWwindow * m_window = NULL;
	vr::IVRSystem* m_vrSystem = NULL;
	std::unique_ptr<CInputGLFW>m_input;
	std::unique_ptr<COpenGLRenderer> m_renderer;

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;
	uint32_t m_viewPortSize[2];
	bool m_vrMode = false;

	static bool m_visible;
};