#pragma once
#include "../view/IGameWindow.h"
#include <memory>

class COpenGLRenderer;

class CGameWindowGLUT : public IGameWindow
{
public:
	CGameWindowGLUT();
	~CGameWindowGLUT();
	void LaunchMainLoop() override;
	void DoOnDrawScene(std::function<void()> const& handler) override;
	void DoOnResize(std::function<void(int, int)> const& handler) override;
	void DoOnShutdown(std::function<void()> const& handler) override;
	void ResizeWindow(int width, int height) override;
	void SetTitle(std::wstring const& title) override;
	void ToggleFullscreen() override;
	void EnableMultisampling(bool enable, int level = 1.0f) override;
	bool EnableVRMode(bool show, VRViewportFactory const&) override;
	IInput& GetInput() override;
	IRenderer& GetRenderer() override;
	IViewHelper& GetViewHelper() override;
	void GetWindowSize(int& width, int& height) override;
private:
	static void OnTimer(int value);
	static void OnChangeState(int state);
	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnShutdown();

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;
	std::unique_ptr<IInput>m_input;
	std::unique_ptr<COpenGLRenderer> m_renderer;

	bool m_2dMode;

	static bool m_visible;
};