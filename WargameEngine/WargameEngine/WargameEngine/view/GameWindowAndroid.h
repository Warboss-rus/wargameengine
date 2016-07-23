#pragma once
#include <EGL/egl.h>
#include "..\..\WargameEngine\view\IGameWindow.h"
#include "..\..\WargameEngine\view\OpenGLESRenderer.h"

struct ANativeWindow;
struct android_app;

class CGameWindowAndroid : public IGameWindow
{
public:
	CGameWindowAndroid(android_app* app);
	~CGameWindowAndroid();

	void Init(ANativeWindow * window);
	void DrawFrame();
	void Shutdown();
	void SetActive(bool active);

	virtual void LaunchMainLoop() override;

	virtual void DoOnDrawScene(std::function<void() > const& handler) override;
	virtual void DoOnResize(std::function<void(int, int) > const& handler) override;
	virtual void DoOnShutdown(std::function<void() > const& handler) override;
	virtual void ResizeWindow(int width, int height) override;
	virtual void SetTitle(std::wstring const& title) override;
	virtual void ToggleFullscreen() override;
	virtual IInput& ResetInput() override;
	virtual IRenderer& GetRenderer() override;
	virtual IViewHelper& GetViewHelper() override;
	virtual void EnableMultisampling(bool enable, int level = 1.0f) override;
private:
	std::function<void()> m_onDraw;
	std::function<void(int, int)> m_onResize;
	std::function<void()> m_onShutdown;
	COpenGLESRenderer m_renderer;
	bool m_active;
	bool m_destroyRequested;
	EGLDisplay m_display;
	EGLSurface m_surface;
	EGLContext m_context;
	int32_t m_width;
	int32_t m_height;
	struct android_app* m_app;
};
