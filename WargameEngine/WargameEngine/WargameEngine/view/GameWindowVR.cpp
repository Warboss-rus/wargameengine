#include "GameWindowVR.h"
#define GLFW_EXPOSE_NATIVE_WIN32 1
#define GLFW_EXPOSE_NATIVE_WGL 1
#include <OVR_CAPI_GL.h>
#include <GLFW/glfw3.h>
#include "InputGLFW.h"
#include "OpenGLRenderer.h"
#include "../LogWriter.h"
#include <algorithm>

static CGameWindowVR* g_instance = nullptr;
bool CGameWindowVR::m_visible = true;

#define GL_FRAMEBUFFER_SRGB 0x8DB9

void CGameWindowVR::OnChangeState(GLFWwindow * /*window*/, int state)
{
	m_visible = (state == GLFW_VISIBLE);
}

void CGameWindowVR::OnReshape(GLFWwindow * /*window*/, int width, int height)
{
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
	while (m_window && !glfwWindowShouldClose(m_window))
	{
		if (m_visible)
		{
			g_instance->m_input->UpdateControllers();
			ovrEyeRenderDesc eyeRenderDesc[2];
			ovrVector3f hmdToEyeViewOffset[2];
			ovrHmdDesc hmdDesc = ovr_GetHmdDesc(m_vrSession);
			eyeRenderDesc[0] = ovr_GetRenderDesc(m_vrSession, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
			eyeRenderDesc[1] = ovr_GetRenderDesc(m_vrSession, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);
			hmdToEyeViewOffset[0] = eyeRenderDesc[0].HmdToEyeOffset;
			hmdToEyeViewOffset[1] = eyeRenderDesc[1].HmdToEyeOffset;
			// Initialize our single full screen Fov layer.
			ovrLayerEyeFov layer;
			layer.Header.Type = ovrLayerType_EyeFov;
			layer.Header.Flags = 0;
			layer.ColorTexture[0] = m_swapChain;
			layer.ColorTexture[1] = m_swapChain;
			layer.Fov[0] = eyeRenderDesc[0].Fov;
			layer.Fov[1] = eyeRenderDesc[1].Fov;
			layer.Viewport[0] = { 0, 0, m_viewPortSize[0] / 2, m_viewPortSize[1] };
			layer.Viewport[1] = { m_viewPortSize[0] / 2, 0, m_viewPortSize[0], m_viewPortSize[1] };
			// ld.RenderPose and ld.SensorSampleTime are updated later per frame.

			unsigned int textureId;
			if (g_instance->m_onDraw)
			{
				ovr_GetTextureSwapChainBufferGL(m_vrSession, m_swapChain, -1, &textureId);
				m_renderer->RenderToExistingTexture(textureId, [this] {
					glViewport(0, 0, m_viewPortSize[0] / 2, m_viewPortSize[1]);
					g_instance->m_onDraw();
				});
				m_renderer->RenderToExistingTexture(textureId, [this] {
					glViewport(m_viewPortSize[0] / 2, 0, m_viewPortSize[0], m_viewPortSize[1]);
					g_instance->m_onDraw();
				});
				ovr_CommitTextureSwapChain(m_vrSession, m_swapChain);
			}
			glBindTexture(GL_TEXTURE_2D, textureId);
			m_renderer->SetUpViewport2D();
			m_renderer->RenderArrays(RenderMode::RECTANGLES, { CVector2i(0, 0), {0, m_viewPortSize[1] }, { m_viewPortSize[0], m_viewPortSize[1] }, { m_viewPortSize[0], 0} }, { { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, {1.0f, 0.0f} });
			m_renderer->RestoreViewport();
			glfwSwapBuffers(g_instance->m_window);

			ovrLayerHeader* layers = &layer.Header;
			ovr_SubmitFrame(m_vrSession, 0, NULL, &layers, 1);
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
	m_window = glfwCreateWindow(600, 600, "WargameEngine", monitor, NULL);

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

std::string GetErrorMessage(ovrResult result)
{
	switch (result)
	{
	case ovrError_Initialize:
		return "Generic initialization error.";
	case ovrError_LibLoad:
		return "Couldn't load LibOVRRT.";
	case ovrError_LibVersion:
		return "LibOVRRT version incompatibility.";
	case ovrError_ServiceConnection:
		return " Couldn't connect to the OVR Service.";
	case ovrError_ServiceVersion:
		return "OVR Service version incompatibility.";
	case ovrError_IncompatibleOS:
		return "The operating system version is incompatible.";
	case ovrError_DisplayInit:
		return "Unable to initialize the HMD display.";
	case ovrError_ServerStart:
		return "Unable to start the server. Is it already running?";
	case ovrError_Reinitialization:
		return "Attempted to re-initialize with a different version.";
	default:
		return "Unknown error";
	}
}

ovrResult LogError(ovrResult result, std::string const& prefix = "OVR error. ")
{
	if (OVR_FAILURE(result))
	{
		LogWriter::WriteLine(prefix + GetErrorMessage(result));
	}
	return result;
}

CGameWindowVR::CGameWindowVR()
{
	g_instance = this;

	LogError(ovr_Initialize(NULL));

	ovrGraphicsLuid luid;
	LogError(ovr_Create(&m_vrSession, &luid));
	auto hdmDesc = ovr_GetHmdDesc(m_vrSession);

	ovrSizei recommenedTex0Size = ovr_GetFovTextureSize(m_vrSession, ovrEye_Left,
		hdmDesc.DefaultEyeFov[0], 1.0f);
	ovrSizei recommenedTex1Size = ovr_GetFovTextureSize(m_vrSession, ovrEye_Right,
		hdmDesc.DefaultEyeFov[1], 1.0f);
	
	ovrTextureSwapChainDesc desc = {};
	desc.Type = ovrTexture_2D;
	desc.ArraySize = 1;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.Width = recommenedTex0Size.w + recommenedTex1Size.w;
	desc.Height = std::max(recommenedTex0Size.h, recommenedTex1Size.h);
	desc.MipLevels = 1;
	desc.SampleCount = 1;
	desc.StaticImage = ovrFalse;
	m_viewPortSize[0] = desc.Width;
	m_viewPortSize[1] = desc.Height;
	if (LogError(ovr_CreateTextureSwapChainGL(m_vrSession, &desc, &m_swapChain) == ovrSuccess))
	{
		// Sample texture access:
		unsigned int texId;
		ovr_GetTextureSwapChainBufferGL(m_vrSession, m_swapChain, -1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
	}
	
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 16);

	CreateNewWindow();

	m_renderer = std::make_unique<COpenGLRenderer>();

	glEnable(GL_FRAMEBUFFER_SRGB);
}

CGameWindowVR::~CGameWindowVR()
{
	glfwTerminate();
	ovr_DestroyTextureSwapChain(m_vrSession, m_swapChain);
	ovr_Destroy(m_vrSession);
	ovr_Shutdown();
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

