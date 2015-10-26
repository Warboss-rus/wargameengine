#pragma once
#include <functional>
#include "IInput.h"
#include "IRenderer.h"
#include "IViewHelper.h"

class IGameWindow
{
public:
	virtual ~IGameWindow() {}

	virtual void LaunchMainLoop() = 0;
	virtual void DoOnDrawScene(std::function<void()> const& handler) = 0;
	virtual void DoOnResize(std::function<void(int, int)> const& handler) = 0;
	virtual void DoOnShutdown(std::function<void()> const& handler) = 0;
	virtual void ResizeWindow(int width, int height) = 0;
	virtual void SetTitle(std::string const& title) = 0;
	virtual void ToggleFullscreen() = 0;
	virtual void Enter2DMode() = 0;
	virtual void Leave2DMode() = 0;
	virtual IInput& ResetInput() = 0;
	virtual IRenderer& GetRenderer() = 0;
	virtual IViewHelper& GetViewHelper() = 0;
	virtual void EnableMultisampling(bool enable, int level = 1.0f) = 0;
};