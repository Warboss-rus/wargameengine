#pragma once
#include <functional>

class CGameWindow
{
public:
	void Init();
	void Clear();
	void DoOnDrawScene(std::function<void()> const& handler);
	void DoOnResize(std::function<void(int, int)> const& handler);
	void DoOnShutdown(std::function<void()> const& handler);
	void ResizeWindow(int width, int height);
	void SetTitle(std::string const& title);
	void ToggleFullscreen();
	void Enter2DMode();
	void Leave2DMode();
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