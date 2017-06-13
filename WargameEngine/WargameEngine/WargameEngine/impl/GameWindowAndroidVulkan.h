#pragma once
#include "..\view\IWindow.h"
#include "VulkanRenderer.h"
#include "InputAndroid.h"

struct ANativeWindow;
struct android_app;

class CGameWindowAndroidVulkan : public wargameEngine::view::IWindow
{
public:
	CGameWindowAndroidVulkan(android_app* app);
	void Init(ANativeWindow * window);
	void DrawFrame();
	void Shutdown();
	void SetActive(bool active);
	void HandleInput(AInputEvent* event);

	void LaunchMainLoop() override;
	void DoOnDrawScene(std::function<void() > const& handler) override;
	void DoOnResize(std::function<void(int, int) > const& handler) override;
	void DoOnShutdown(std::function<void() > const& handler) override;
	void ResizeWindow(int width, int height) override;
	void SetTitle(std::wstring const& title) override;
	void ToggleFullscreen() override;
	bool EnableVRMode(bool show, VRViewportFactory const& viewportFactory = VRViewportFactory()) override;
	wargameEngine::view::IInput& GetInput() override;
	wargameEngine::view::IRenderer& GetRenderer() override;
	wargameEngine::view::IViewHelper& GetViewHelper() override;
	void EnableMultisampling(bool enable, int level = 1.0f) override;
	void GetWindowSize(int& width, int& height) override;
private:
	android_app* m_app;
	CVulkanRenderer m_renderer;
	CInputAndroid m_input;
	bool m_active = true;
	bool m_destroyRequested = false;
	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;
};