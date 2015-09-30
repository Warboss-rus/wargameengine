#pragma once
#include "IGameWindow.h"

class CGameWindowGLUT : public IGameWindow
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
	virtual void EnableMultisampling(bool enable, int level = 1.0f) override;
private:
	static void OnTimer(int value);
	static void OnChangeState(int state);
	static void OnDrawScene();
	static void OnReshape(int width, int height);
	static void OnShutdown();

	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;

	bool m_2dMode;

	static bool m_visible;
};