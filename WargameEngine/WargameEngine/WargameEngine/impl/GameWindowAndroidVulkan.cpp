#define VK_USE_PLATFORM_ANDROID_KHR
#include "GameWindowAndroidVulkan.h"
#include "..\..\WargameEngineMobile\WargameEngineMobile.NativeActivity\android_native_app_glue.h"

CGameWindowAndroidVulkan::CGameWindowAndroidVulkan(android_app* app)
	: m_app(app), m_renderer({"VK_KHR_surface", "VK_KHR_android_surface"})
{
}

void CGameWindowAndroidVulkan::Init(ANativeWindow * window)
{
	VkAndroidSurfaceCreateInfoKHR createInfo{
		VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR, nullptr,	0, window };
	VkSurfaceKHR surface;
	vkCreateAndroidSurfaceKHR(m_renderer.GetInstance(), &createInfo, nullptr, &surface);
	m_renderer.SetSurface(surface);
	m_onResize(ANativeWindow_getWidth(window), ANativeWindow_getHeight(window));
}

void CGameWindowAndroidVulkan::DrawFrame()
{
	if (m_active && m_app->window)
	{
		m_renderer.AcquireImage();
		m_onDraw();
		m_renderer.Present();
	}
}

void CGameWindowAndroidVulkan::Shutdown()
{

}

void CGameWindowAndroidVulkan::SetActive(bool active)
{
	m_active = active;
}

void CGameWindowAndroidVulkan::HandleInput(AInputEvent* event)
{
	m_input.HandleInput(event);
}

void CGameWindowAndroidVulkan::LaunchMainLoop()
{
	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(m_active ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
		{
			// Process this event.
			if (source != NULL)
			{
				source->process(m_app, source);
			}

			// If a sensor has data, process it now.

			// Check if we are exiting.
			if (m_destroyRequested)
			{
				return;
			}
		}

		DrawFrame();
	}
	Shutdown();
}

void CGameWindowAndroidVulkan::DoOnDrawScene(std::function<void()> const& handler)
{
	m_onDraw = handler;
}

void CGameWindowAndroidVulkan::DoOnResize(std::function<void(int, int)> const& handler)
{
	m_onResize = handler;
}

void CGameWindowAndroidVulkan::DoOnShutdown(std::function<void() > const& handler)
{
	m_onShutdown = handler;
}

void CGameWindowAndroidVulkan::ResizeWindow(int width, int height)
{
	//you can't change resolution on android
}

void CGameWindowAndroidVulkan::SetTitle(std::wstring const& title)
{
	//you can't change window title on android
}

void CGameWindowAndroidVulkan::ToggleFullscreen()
{
	//there is no windowed mode on android
}

bool CGameWindowAndroidVulkan::EnableVRMode(bool show, VRViewportFactory const& viewportFactory /*= VRViewportFactory()*/)
{
	throw std::logic_error("The method or operation is not implemented.");
}

wargameEngine::view::IInput& CGameWindowAndroidVulkan::GetInput()
{
	return m_input;
}

wargameEngine::view::IRenderer& CGameWindowAndroidVulkan::GetRenderer()
{
	return m_renderer;
}

wargameEngine::view::IViewHelper& CGameWindowAndroidVulkan::GetViewHelper()
{
	return m_renderer;
}

void CGameWindowAndroidVulkan::EnableMultisampling(bool enable, int level /*= 1.0f*/)
{
	
}

void CGameWindowAndroidVulkan::GetWindowSize(int& width, int& height)
{
	if (m_app->window)
	{
		width = ANativeWindow_getWidth(m_app->window);
		height = ANativeWindow_getHeight(m_app->window);
	}
	else
	{
		width = 1920;
		height = 1080;
	}
}
