#pragma once
#include "../view/IGameWindow.h"
#include <memory>
#include "vr/gvr/capi/include/gvr.h"
#include "InputAndroid.h"
#include "OpenGLESRenderer.h"

class View;

class CGvrGameWindow : public IGameWindow
{
public:
	CGvrGameWindow(gvr_context* gvr_context);

	void Init();
	void Draw();
	void TriggerEvent();
	void Pause();
	void Resume();
	CInputAndroid& GetAndroidInput();

	void LaunchMainLoop() override;
	void DoOnDrawScene(std::function<void() > const& handler) override;
	void DoOnResize(std::function<void(int, int) > const& handler) override;
	void DoOnShutdown(std::function<void() > const& handler) override;
	void ResizeWindow(int width, int height) override;
	void SetTitle(std::wstring const& title) override;
	void ToggleFullscreen() override;
	bool EnableVRMode(bool show, VRViewportFactory const&) override;
	IInput& GetInput() override;
	IRenderer& GetRenderer() override;
	IViewHelper& GetViewHelper() override;
	void EnableMultisampling(bool enable, int level = 1.0f) override;
	void GetWindowSize(int& width, int& height) override;

private:
	std::unique_ptr<gvr::GvrApi> m_gvr_api;
	gvr::BufferViewportList m_viewport_list;
	gvr::SwapChain m_swapchain;
	gvr::BufferViewport m_viewport_left;
	gvr::BufferViewport m_viewport_right;
	bool m_multiview = false;
	std::unique_ptr<IShaderProgram> m_multiviewDefaultProgram;

	CInputAndroid m_input;
	COpenGLESRenderer m_renderer;

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;
};