#include "GameWindowVR.h"
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "InputGLFW.h"
#include "OpenGLRenderer.h"
#include "../LogWriter.h"
#include <openvr.h>

static CGameWindowVR* g_instance = nullptr;
bool CGameWindowVR::m_visible = true;

using namespace vr;

#define GL_DEPTH_COMPONENT24 0x81A6

void LogError(HmdError result, std::string const& prefix = "OVR error. ")
{
	if (result != VRInitError_None)
	{
		LogWriter::WriteLine(prefix + VR_GetVRInitErrorAsEnglishDescription(result));
	}
}

void CGameWindowVR::OnChangeState(GLFWwindow * /*window*/, int state)
{
	m_visible = (state == GLFW_VISIBLE);
}

void CGameWindowVR::OnReshape(GLFWwindow * /*window*/, int width, int height)
{
	if (g_instance->m_vrSystem)
	{
		width = g_instance->m_viewPortSize[0];
		height = g_instance->m_viewPortSize[1];
	}
	g_instance->m_renderer->OnResize(width, height);
	if (g_instance->m_onResize)
	{
		g_instance->m_onResize(width, height);
	}
}

void CGameWindowVR::OnShutdown(GLFWwindow * window)
{
	if (g_instance->m_onShutdown)
	{
		g_instance->m_onShutdown();
	}
	glfwDestroyWindow(window);
	g_instance->m_window = nullptr;
}

void CGameWindowVR::LaunchMainLoop()
{
	unsigned int textures[2];
	glGenTextures(2, textures);
	unsigned int depthTextures[2];
	glGenTextures(2, depthTextures);
	unsigned int buffers[2];
	glGenFramebuffers(2, buffers);
	for (int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_viewPortSize[0], m_viewPortSize[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, depthTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_viewPortSize[0], m_viewPortSize[1], 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
		glBindFramebuffer(GL_FRAMEBUFFER, buffers[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextures[i], 0);
		GLenum l_GLDrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, l_GLDrawBuffers);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			LogWriter::WriteLine("framebuffer error code=" + std::to_string(status));
		}
	}
	auto compositor = m_vrSystem ? VRCompositor() : nullptr;
	TrackedDevicePose_t m_rTrackedDevicePose[k_unMaxTrackedDeviceCount];
	TrackedDevicePose_t m_rTrackedGamePose[k_unMaxTrackedDeviceCount];
	while (m_window && !glfwWindowShouldClose(m_window))
	{
		if (m_visible || compositor)
		{
			m_renderer->OnResize(m_viewPortSize[0], m_viewPortSize[1]);
			if (compositor)
			{
				compositor->WaitGetPoses(m_rTrackedDevicePose, k_unMaxTrackedDeviceCount, m_rTrackedGamePose, k_unMaxTrackedDeviceCount);
			}
			g_instance->m_input->UpdateControllers();
			if (g_instance->m_onDraw)
			{
				for (size_t i = 0; i < 2; ++i)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, buffers[i]);
					//modify camera
					g_instance->m_onDraw();
				}
				glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
			}
			int width, height;
			glfwGetFramebufferSize(m_window, &width, &height);
			m_renderer->OnResize(width, height);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			width /= 2;
			for (int i = 0; i < 2; ++i)
			{
				glBindFramebuffer(GL_READ_FRAMEBUFFER, buffers[i]);
				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glBlitFramebuffer(0, 0, m_viewPortSize[0], m_viewPortSize[1], width * i, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			}
			glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);
			if (compositor)
			{
				for (int i = 0; i < 2; ++i)
				{
					const vr::Texture_t tex = { reinterpret_cast<void*>(textures[i]), API_OpenGL, ColorSpace_Gamma };
					EVRCompositorError err = compositor->Submit(vr::EVREye(i), &tex);
					if (err)
					{
						return;
					}
				}
				glFlush();
				compositor->PostPresentHandoff();
			}
			glfwSwapBuffers(g_instance->m_window);
		}
		glfwPollEvents();
	}
	glfwTerminate();
}

void CGameWindowVR::CreateNewWindow(GLFWmonitor * monitor /*= NULL*/)
{
	if (m_window)
	{
		glfwDestroyWindow(m_window);
	}
	m_window = glfwCreateWindow(1280, 720, "WargameEngine", monitor, NULL);

	glfwMakeContextCurrent(m_window);

	glfwSetWindowSizeCallback(m_window, &OnReshape);
	glfwSetKeyCallback(m_window, &CInputGLFW::OnKeyboard);
	glfwSetCharCallback(m_window, &CInputGLFW::OnCharacter);
	glfwSetMouseButtonCallback(m_window, &CInputGLFW::OnMouse);
	glfwSetCursorPosCallback(m_window, &CInputGLFW::OnMouseMove);
	glfwSetScrollCallback(m_window, &CInputGLFW::OnScroll);
	glfwSetWindowCloseCallback(m_window, &CGameWindowVR::OnShutdown);
	glfwSetWindowIconifyCallback(m_window, &OnChangeState);
}

CGameWindowVR::CGameWindowVR()
{
	g_instance = this;

	HmdError err;
	m_vrSystem = VR_Init(&err, VRApplication_Scene);
	LogError(err);
	m_viewPortSize[0] = 1280;
	m_viewPortSize[1] = 720;
	if (m_vrSystem)
	{
		m_vrSystem->GetRecommendedRenderTargetSize(&m_viewPortSize[0], &m_viewPortSize[1]);
		auto compositor = VR_GetGenericInterface(IVRCompositor_Version, &err);
		LogError(err);
		if (!compositor)
		{
			VR_Shutdown();
			m_vrSystem = nullptr;
		}
	}
	
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 16);

	CreateNewWindow();

	m_renderer = std::make_unique<COpenGLRenderer>();
}

CGameWindowVR::~CGameWindowVR()
{
	glfwTerminate();
	VR_Shutdown();
}

void CGameWindowVR::DoOnDrawScene(std::function<void()> const& handler)
{
	m_onDraw = handler;
}

void CGameWindowVR::DoOnResize(std::function<void(int, int)> const& handler)
{
	m_onResize = handler;
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	OnReshape(m_window, width, height);
}

void CGameWindowVR::DoOnShutdown(std::function<void()> const& handler)
{
	m_onShutdown = handler;
}

void CGameWindowVR::ResizeWindow(int width, int height)
{
	glfwSetWindowSize(m_window, width, height);
}

void CGameWindowVR::SetTitle(std::string const& title)
{
	glfwSetWindowTitle(m_window, title.c_str());
}

void CGameWindowVR::ToggleFullscreen()
{
	CreateNewWindow(glfwGetPrimaryMonitor());
}

void CGameWindowVR::EnableMultisampling(bool enable, int level /*= 1.0f*/)
{
	m_renderer->EnableMultisampling(enable);
	glfwWindowHint(GLFW_SAMPLES, level);
}

IInput& CGameWindowVR::ResetInput()
{
	m_input = std::make_unique<CInputGLFW>(m_window);
	return *m_input;
}

IRenderer& CGameWindowVR::GetRenderer()
{
	return *m_renderer;
}

IViewHelper& CGameWindowVR::GetViewHelper()
{
	return *m_renderer;
}

