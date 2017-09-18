#pragma once
#include "../view/IWindow.h"
#include <memory>

class CGameWindowDirectX : public wargameEngine::view::IWindow
{
public:
	CGameWindowDirectX();
	virtual ~CGameWindowDirectX();

	virtual void LaunchMainLoop() override;
	virtual void DoOnDrawScene(std::function<void() > const& handler) override;
	virtual void DoOnResize(std::function<void(int, int) > const& handler) override;
	virtual void DoOnShutdown(std::function<void() > const& handler) override;
	virtual void ResizeWindow(int width, int height) override;
	virtual void SetTitle(std::wstring const& title) override;
	virtual void ToggleFullscreen() override;
	virtual bool EnableVRMode(bool show, VRViewportFactory const&) override;
	virtual wargameEngine::view::IInput& GetInput() override;
	virtual wargameEngine::view::IRenderer& GetRenderer() override;
	virtual wargameEngine::view::IViewHelper& GetViewHelper() override;
	virtual void EnableMultisampling(bool enable, int level = 1.0f) override;
	virtual void GetWindowSize(int& width, int& height) override;
private:
	struct Impl;
	std::unique_ptr<Impl> m_pImpl;
};