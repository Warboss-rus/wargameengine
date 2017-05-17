#pragma once
#include "../view/IGameWindow.h"
#include <functional>
#include <memory>

class IOpenGLRenderer;
class CInputGLFW;
struct GLFWwindow;
struct GLFWmonitor;

namespace vr
{
class IVRSystem;
}

class CGameWindowGLFW : public wargameEngine::view::IGameWindow
{
public:
	CGameWindowGLFW();
	~CGameWindowGLFW();
	void LaunchMainLoop() override;
	void DoOnDrawScene(std::function<void()> const& handler) override;
	void DoOnResize(std::function<void(int, int)> const& handler) override;
	void DoOnShutdown(std::function<void()> const& handler) override;
	void ResizeWindow(int width, int height) override;
	void SetTitle(std::wstring const& title) override;
	void ToggleFullscreen() override;
	void EnableMultisampling(bool enable, int level = 1.0f) override;
	bool EnableVRMode(bool show, VRViewportFactory const& viewportFactory = VRViewportFactory()) override;
	wargameEngine::view::IInput& GetInput() override;
	wargameEngine::view::IRenderer& GetRenderer() override;
	wargameEngine::view::IViewHelper& GetViewHelper() override;
	void GetWindowSize(int& width, int& height) override;

	CInputGLFW& Input() { return *m_input; }

private:
	void CreateNewWindow(GLFWmonitor* monitor = NULL);

	static void OnChangeState(GLFWwindow* window, int state);
	static void OnReshape(GLFWwindow* window, int width, int height);
	static void OnShutdown(GLFWwindow* window);

	GLFWwindow* m_window = NULL;
	std::unique_ptr<CInputGLFW> m_input;
	std::unique_ptr<IOpenGLRenderer> m_renderer;
	bool m_vulkanRenderer = false;
	bool m_visible = true;

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;

	vr::IVRSystem* m_vrSystem = NULL;
	std::vector<const wargameEngine::view::ICachedTexture*> m_eyeTextures;
};