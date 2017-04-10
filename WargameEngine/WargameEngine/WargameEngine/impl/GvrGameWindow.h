#pragma once
#include "../view/IGameWindow.h"
#include <memory>
#include "vr/gvr/capi/include/gvr.h"
#include "InputAndroid.h"
#include "OpenGLESRenderer.h"

class CGameView;

class CGvrGameWindow : public IGameWindow
{
public:
	CGvrGameWindow(gvr_context* gvr_context);

	void Init();
	void Draw();
	void TriggerEvent();
	void Pause();
	void Resume();
	CInputAndroid& GetInput();

	virtual void LaunchMainLoop() override;
	virtual void DoOnDrawScene(std::function<void() > const& handler) override;
	virtual void DoOnResize(std::function<void(int, int) > const& handler) override;
	virtual void DoOnShutdown(std::function<void() > const& handler) override;
	virtual void ResizeWindow(int width, int height) override;
	virtual void SetTitle(std::wstring const& title) override;
	virtual void ToggleFullscreen() override;
	virtual bool EnableVRMode(bool show, VRViewportFactory const&) override;
	virtual IInput& ResetInput() override;
	virtual IRenderer& GetRenderer() override;
	virtual IViewHelper& GetViewHelper() override;
	virtual void EnableMultisampling(bool enable, int level = 1.0f) override;
	virtual void GetWindowSize(int& width, int& height) override;
private:
	std::unique_ptr<gvr::GvrApi> m_gvr_api;
	gvr::BufferViewportList m_viewport_list;
	gvr::SwapChain m_swapchain;
	gvr::BufferViewport m_viewport_left;
	gvr::BufferViewport m_viewport_right;

	CInputAndroid m_input;
	COpenGLESRenderer m_renderer;

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;
};