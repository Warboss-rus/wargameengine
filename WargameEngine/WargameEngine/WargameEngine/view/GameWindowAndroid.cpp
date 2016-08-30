#include "GameWindowAndroid.h"
#include <GLES/gl.h>
#include "android\looper.h"
#include "../../WargameEngineMobile/WargameEngineMobile.NativeActivity/android_native_app_glue.h"
#include "../LogWriter.h"

CGameWindowAndroid::CGameWindowAndroid(android_app* app)
	: m_app(app)
{
}

CGameWindowAndroid::~CGameWindowAndroid()
{
}

void CGameWindowAndroid::Init(ANativeWindow * window)
{
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_BIND_TO_TEXTURE_RGBA, EGL_TRUE,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 8,
		EGL_STENCIL_SIZE, 0,
		EGL_NONE
	};
	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(m_display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	* sample, we have a very simplified selection process, where we pick
	* the first EGLConfig that matches our criteria */
	EGLint numConfigs;
	EGLConfig config;
	eglChooseConfig(m_display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	EGLint format;
	eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(window, 0, 0, format);

	auto createContext = [this, &config](int glVersion)->bool {
		EGLint attrib_list[] = { EGL_CONTEXT_CLIENT_VERSION, glVersion, EGL_NONE };
		m_context = eglCreateContext(m_display, config, NULL, attrib_list);
		return m_context;
	};

	m_surface = eglCreateWindowSurface(m_display, config, window, NULL);
	if (!createContext(3) && !createContext(2) && !createContext(1))
	{
		throw std::runtime_error("eglCreateContext failed");
	}

	if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context))
	{
		throw std::runtime_error("Unable to eglMakeCurrent");
	}

	int width, height;
	eglQuerySurface(m_display, m_surface, EGL_WIDTH, &width);
	eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &height);

	if (m_onResize)
	{
		m_onResize(width, height);
	}

	// Initialize GL state.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	m_active = true;
}

void CGameWindowAndroid::DrawFrame()
{
	if (m_active) 
	{
		if (m_onUpdate)
		{
			m_onUpdate();
		}
		if (m_onDraw)
		{
			m_onDraw(RenderEye::NONE);
		}

		// Drawing is throttled to the screen update rate, so there
		// is no need to do timing here.
		eglSwapBuffers(m_display, m_surface);
	}
}

void CGameWindowAndroid::LaunchMainLoop()
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

void CGameWindowAndroid::HandleInput(AInputEvent* event)
{
	m_input.HandleInput(event);
}

void CGameWindowAndroid::DoOnUpdate(std::function<void() > const& handler)
{
	m_onUpdate = handler;
}

void CGameWindowAndroid::DoOnDrawScene(std::function<void(RenderEye) > const& handler)
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

void CGameWindowAndroid::SetTitle(std::wstring const& /*title*/)
{
	//you can't change window title on android
}

void CGameWindowAndroid::ToggleFullscreen()
{
	//there is no windowed mode on android
}

void CGameWindowAndroid::EnableVRMode(bool /*show*/)
{
	LogWriter::WriteLine("GameWindowAndroid does not support VR mode, use GameWindowVR instead");
}

IInput& CGameWindowAndroid::ResetInput()
{
	m_input = CInputAndroid();
	return m_input;
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
