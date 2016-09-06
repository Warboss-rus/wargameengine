#include "GameWindowGLFW.h"
#ifdef VULKAN_API
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#include "InputGLFW.h"
#include "OpenGLRenderer.h"
#include "../Utils.h"
#include "../LogWriter.h"

static CGameWindowGLFW* g_instance = nullptr;
bool CGameWindowGLFW::m_visible = true;

void CGameWindowGLFW::OnChangeState(GLFWwindow * /*window*/, int state)
{
	m_visible = (state == GLFW_VISIBLE);
}

void CGameWindowGLFW::OnReshape(GLFWwindow * /*window*/, int width, int height)
{
	g_instance->m_renderer->OnResize(width, height);
	if (g_instance->m_onResize)
	{
		g_instance->m_onResize(width, height);
	}
}

void CGameWindowGLFW::OnShutdown(GLFWwindow * window)
{
	if (g_instance->m_onShutdown)
	{
		g_instance->m_onShutdown();
	}
	glfwDestroyWindow(window);
	g_instance->m_window = nullptr;
}

void CGameWindowGLFW::LaunchMainLoop()
{
	while (m_window && !glfwWindowShouldClose(m_window))
	{
		if (m_visible)
		{
			g_instance->m_input->UpdateControllers();
			if (g_instance->m_onDraw)
			{
				g_instance->m_onDraw();
			}
			glfwSwapBuffers(g_instance->m_window);
		}
		glfwPollEvents();
	}
	glfwTerminate();
}

void CGameWindowGLFW::CreateNewWindow(GLFWmonitor * monitor /*= NULL*/)
{
	if (m_window)
	{
		glfwDestroyWindow(m_window);
	}
	m_window = glfwCreateWindow(600, 600, "WargameEngine", monitor, NULL);
#ifdef VULKAN_API
	VkSurfaceKHR surface;
	VkResult err = glfwCreateWindowSurface(instance, window, NULL, &surface);
#else
	glfwMakeContextCurrent(m_window);
#endif

	glfwSetWindowSizeCallback(m_window, &OnReshape);
	glfwSetKeyCallback(m_window, &CInputGLFW::OnKeyboard);
	glfwSetCharCallback(m_window, &CInputGLFW::CharacterCallback);
	glfwSetMouseButtonCallback(m_window, &CInputGLFW::OnMouse);
	glfwSetCursorPosCallback(m_window, &CInputGLFW::MouseMoveCallback);
	glfwSetScrollCallback(m_window, &CInputGLFW::OnScroll);
	glfwSetWindowCloseCallback(m_window, &CGameWindowGLFW::OnShutdown);
	glfwSetWindowIconifyCallback(m_window, &OnChangeState);
}

CGameWindowGLFW::CGameWindowGLFW()
{
	g_instance = this;

	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 16);
#ifdef VULKAN_API
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

	CreateNewWindow();

	m_renderer = std::make_unique<COpenGLRenderer>();
}

CGameWindowGLFW::~CGameWindowGLFW()
{
	glfwTerminate();
}

void CGameWindowGLFW::DoOnDrawScene(std::function<void()> const& handler)
{
	m_onDraw = handler;
}

void CGameWindowGLFW::DoOnResize(std::function<void(int, int)> const& handler)
{
	m_onResize = handler;
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	OnReshape(m_window, width, height);
}

void CGameWindowGLFW::DoOnShutdown(std::function<void()> const& handler)
{
	m_onShutdown = handler;
}

void CGameWindowGLFW::ResizeWindow(int width, int height)
{
	glfwSetWindowSize(m_window, width, height);
}

void CGameWindowGLFW::SetTitle(std::wstring const& title)
{
	glfwSetWindowTitle(m_window, WStringToUtf8(title).c_str());
}

void CGameWindowGLFW::ToggleFullscreen()
{
	CreateNewWindow(glfwGetPrimaryMonitor());
}

void CGameWindowGLFW::EnableMultisampling(bool enable, int level /*= 1.0f*/)
{
	m_renderer->EnableMultisampling(enable);
	glfwWindowHint(GLFW_SAMPLES, level);
}

void CGameWindowGLFW::EnableVRMode(bool)
{
	LogWriter::WriteLine("GameWindowGLFW does not support VR mode, use GameWindowVR instead");
}

IInput& CGameWindowGLFW::ResetInput()
{
	m_input = std::make_unique<CInputGLFW>(m_window);
	return *m_input;
}

IRenderer& CGameWindowGLFW::GetRenderer()
{
	return *m_renderer;
}

IViewHelper& CGameWindowGLFW::GetViewHelper()
{
	return *m_renderer;
}

