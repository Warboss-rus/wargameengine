#include "GameWindowAndroid.h"
#include <GLES/gl.h>
#include "android\looper.h"
#include "../../WargameEngineMobile/WargameEngineMobile.NativeActivity/android_native_app_glue.h"

CGameWindowAndroid* g_instance = nullptr;

CGameWindowAndroid::CGameWindowAndroid(android_app* app)
	: m_app(app)
{
	g_instance = this;
}

CGameWindowAndroid::~CGameWindowAndroid()
{
	g_instance = nullptr;
}

void CGameWindowAndroid::Init(ANativeWindow * window)
{
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;

	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(m_display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	* sample, we have a very simplified selection process, where we pick
	* the first EGLConfig that matches our criteria */
	eglChooseConfig(m_display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(window, 0, 0, format);

	m_surface = eglCreateWindowSurface(m_display, config, window, NULL);
	m_context = eglCreateContext(m_display, config, NULL, NULL);

	if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) == EGL_FALSE)
	{
		throw std::runtime_error("Unable to eglMakeCurrent");
	}

	eglQuerySurface(m_display, m_surface, EGL_WIDTH, &w);
	eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &h);

	// Initialize GL state.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	m_active = true;
}

void CGameWindowAndroid::DrawFrame()
{
	if (m_active) {
		m_onDraw();

		// Drawing is throttled to the screen update rate, so there
		// is no need to do timing here.
		eglSwapBuffers(m_display, m_surface);
	}
}

void CGameWindowAndroid::LaunchMainLoop()
{
	m_active = true;

	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(m_active ? 0 : -1, NULL, &events,
			(void**)&source)) >= 0) {

			// Process this event.
			if (source != NULL) {
				source->process(m_app, source);
			}

			// If a sensor has data, process it now.

			// Check if we are exiting.
			if (m_destroyRequested != 0) {
				return;
			}
		}

		DrawFrame();
	}
	Shutdown();
}

void CGameWindowAndroid::Shutdown()
{
	m_destroyRequested = true;
	if (m_display != EGL_NO_DISPLAY) {
		eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (m_context != EGL_NO_CONTEXT) {
			eglDestroyContext(m_display, m_context);
		}
		if (m_surface != EGL_NO_SURFACE) {
			eglDestroySurface(m_display, m_surface);
		}
		eglTerminate(m_display);
	}
	m_active = false;
	m_display = EGL_NO_DISPLAY;
	m_context = EGL_NO_CONTEXT;
	m_surface = EGL_NO_SURFACE;
}

void CGameWindowAndroid::SetActive(bool active)
{
	m_active = active;
}

void CGameWindowAndroid::DoOnDrawScene(std::function<void() > const& handler)
{
	m_onDraw = handler;
}

void CGameWindowAndroid::DoOnResize(std::function<void(int, int) > const& handler)
{
	m_onResize = handler;
}

void CGameWindowAndroid::DoOnShutdown(std::function<void() > const& handler)
{
	m_onShutdown = handler;
}

void CGameWindowAndroid::ResizeWindow(int /*width*/, int /*height*/)
{
	//you can't change resolution on android
}

void CGameWindowAndroid::SetTitle(std::wstring const& title)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CGameWindowAndroid::ToggleFullscreen()
{
	//there is no windowed mode on android
}

IInput& CGameWindowAndroid::ResetInput()
{
	throw std::logic_error("The method or operation is not implemented.");
}

IRenderer& CGameWindowAndroid::GetRenderer()
{
	return m_renderer;
}

IViewHelper& CGameWindowAndroid::GetViewHelper()
{
	return m_renderer;
}

void CGameWindowAndroid::EnableMultisampling(bool enable, int level /*= 1.0f*/)
{
	m_renderer.EnableMultisampling(enable);
}
