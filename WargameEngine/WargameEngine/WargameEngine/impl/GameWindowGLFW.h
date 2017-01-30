#pragma once
#include <functional>
#include <memory>
#include "../view/IGameWindow.h"

class IOpenGLRenderer;
class CInputGLFW;
struct GLFWwindow;
struct GLFWmonitor;
namespace vr
{
	class IVRSystem;
}

class CGameWindowGLFW : public IGameWindow
{
public:
	CGameWindowGLFW();
	~CGameWindowGLFW();
	virtual void LaunchMainLoop() override;
	virtual void DoOnDrawScene(std::function<void()> const& handler) override;
	virtual void DoOnResize(std::function<void(int, int)> const& handler) override;
	virtual void DoOnShutdown(std::function<void()> const& handler) override;
	virtual void ResizeWindow(int width, int height) override;
	virtual void SetTitle(std::wstring const& title) override;
	virtual void ToggleFullscreen() override;
	virtual void EnableMultisampling(bool enable, int level = 1.0f) override;
	virtual bool EnableVRMode(bool show, VRViewportFactory const& viewportFactory = VRViewportFactory()) override;
	virtual IInput& ResetInput() override;
	virtual IRenderer& GetRenderer() override;
	virtual IViewHelper& GetViewHelper() override;
	virtual void GetWindowSize(int& width, int& height) override;
private:
	void CreateNewWindow(GLFWmonitor * monitor = NULL);

	static void OnChangeState(GLFWwindow * window, int state);
	static void OnReshape(GLFWwindow * window, int width, int height);
	static void OnShutdown(GLFWwindow * window);

	GLFWwindow * m_window = NULL;
	std::unique_ptr<CInputGLFW>m_input;
	std::unique_ptr<IOpenGLRenderer> m_renderer;

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;

	static bool m_visible;

	vr::IVRSystem* m_vrSystem = NULL;
	std::vector<const ICachedTexture *> m_eyeTextures;
};