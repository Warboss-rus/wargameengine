#include "GameWindowGLFW.h"
#define RENDERER_NO_VULKAN
#ifndef RENDERER_NO_VULKAN
#include "VulkanRenderer.h"
#endif
#include <GLFW/glfw3.h>
#define VR_API_EXPORT
#include <openvr.h>
#include "InputGLFW.h"
#include "OpenGLRenderer.h"
#include "../Utils.h"
#include "../LogWriter.h"
#include "../view/IViewport.h"
#include "../view/Matrix4.h"
#ifndef RENDERER_NO_LEGACY
#include "LegacyOpenGLRenderer.h"
#endif

using namespace vr;
using namespace wargameEngine;

void LogVRError(HmdError result, std::string const& prefix = "OVR error. ")
{
	if (result != VRInitError_None)
	{
		LogWriter::WriteLine(prefix + VR_GetVRInitErrorAsEnglishDescription(result));
	}
}

void CGameWindowGLFW::OnChangeState(GLFWwindow * window, int state)
{
	auto instance = reinterpret_cast<CGameWindowGLFW*>(glfwGetWindowUserPointer(window));
	instance->m_visible = (state == GLFW_VISIBLE);
}

void CGameWindowGLFW::OnReshape(GLFWwindow * window, int width, int height)
{
	auto instance = reinterpret_cast<CGameWindowGLFW*>(glfwGetWindowUserPointer(window));
	if (instance->m_onResize)
	{
		instance->m_onResize(width, height);
	}
#ifndef RENDERER_NO_VULKAN
	if (instance->m_vulkanRenderer)
	{
		reinterpret_cast<CVulkanRenderer&>(*instance->m_renderer).Resize();
	}
#endif
}

void CGameWindowGLFW::OnShutdown(GLFWwindow * window)
{
	auto instance = reinterpret_cast<CGameWindowGLFW*>(glfwGetWindowUserPointer(window));
	if (instance->m_onShutdown)
	{
		instance->m_onShutdown();
	}
	glfwDestroyWindow(window);
	instance->m_window = nullptr;
}

Matrix4F ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
{
	float matrixObj[16] = {
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	};
	return matrixObj;
}

void CGameWindowGLFW::LaunchMainLoop()
{
	while (m_window && !glfwWindowShouldClose(m_window))
	{
		if (m_visible)
		{
			auto compositor = m_vrSystem ? VRCompositor() : nullptr;
			if (compositor)
			{
				TrackedDevicePose_t m_rTrackedDevicePose[k_unMaxTrackedDeviceCount];
				TrackedDevicePose_t m_rTrackedGamePose[k_unMaxTrackedDeviceCount];
				compositor->WaitGetPoses(m_rTrackedDevicePose, k_unMaxTrackedDeviceCount, m_rTrackedGamePose, k_unMaxTrackedDeviceCount);
				for (int i = 0; i < k_unMaxTrackedDeviceCount; ++i) 
				{
					if (m_rTrackedDevicePose[i].bPoseIsValid)
					{
						auto matrix = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[i].mDeviceToAbsoluteTracking);
						m_input->SetHeadRotation(i, matrix);
					}
				}
			}
			m_input->UpdateControllers();
#ifndef RENDERER_NO_VULKAN
			if (m_vulkanRenderer)
			{
				auto* renderer = reinterpret_cast<CVulkanRenderer*>(m_renderer.get());
				renderer->AcquireImage();
				m_onDraw();
				renderer->Present();
			}
			else 
#endif
			if (m_onDraw)
			{
				m_onDraw();
			}
			if (compositor)
			{
#ifndef RENDERER_NO_VULKAN
				vr::VRVulkanTextureData_t textureData[2];
#endif
				for (int i = 0; i < 2; ++i)
				{
					vr::Texture_t tex;
					if (m_vulkanRenderer)
					{
#ifndef RENDERER_NO_VULKAN
						tex.eType = vr::ETextureType::TextureType_Vulkan;
						auto& t = reinterpret_cast<const CVulkanCachedTexture&>(*m_eyeTextures[i]);
						textureData[i].m_nFormat = t.GetFormat();
						textureData[i].m_nImage = t;
						//TODO: fill the structure
						tex.handle = reinterpret_cast<void*>(&textureData[i]);
#endif
					}
					else
					{
						tex.eType = vr::ETextureType::TextureType_OpenGL;
						unsigned int t = reinterpret_cast<const COpenGlCachedTexture&>(*m_eyeTextures[i]);
						tex.handle = reinterpret_cast<void*>(t);
					}
					tex.eColorSpace = vr::EColorSpace::ColorSpace_Gamma;
					compositor->Submit(vr::EVREye(i), &tex);
				}
				compositor->PostPresentHandoff();
			}
			glfwSwapBuffers(m_window);
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
	if (!m_window)
	{
		return;
	}
	m_input = std::make_unique<CInputGLFW>(m_window);
	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, this);

	glfwSetWindowSizeCallback(m_window, &OnReshape);
	glfwSetKeyCallback(m_window, &CInputGLFW::OnKeyboard);
	glfwSetCharCallback(m_window, &CInputGLFW::CharacterCallback);
	glfwSetMouseButtonCallback(m_window, &CInputGLFW::OnMouse);
	glfwSetCursorPosCallback(m_window, &CInputGLFW::MouseMoveCallback);
	glfwSetScrollCallback(m_window, &CInputGLFW::OnScroll);
	glfwSetWindowCloseCallback(m_window, &CGameWindowGLFW::OnShutdown);
	glfwSetWindowIconifyCallback(m_window, &OnChangeState);
	glfwSetJoystickCallback(&CInputGLFW::JoystickCallback);
}

CGameWindowGLFW::CGameWindowGLFW()
{
	glfwInit();

#ifndef RENDERER_NO_VULKAN
	//Try Vulkan first
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	try
	{
		if (!glfwVulkanSupported())
		{
			throw std::runtime_error("Vulkan is not supported");
		}
		std::vector<const char*> instanceExtensions = {
#ifdef _DEBUG
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
		};
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		{
			instanceExtensions.push_back(glfwExtensions[i]);
		}
		auto renderer = std::make_unique<CVulkanRenderer>(instanceExtensions);
		CreateNewWindow();
		VkSurfaceKHR surface;
		VkResult result = glfwCreateWindowSurface(renderer->GetInstance(), m_window, nullptr, &surface);
		if (result)
		{
			throw std::runtime_error("Cannot get window surface");
		}
		renderer->SetSurface(surface);
		m_renderer = std::move(renderer);
		m_vulkanRenderer = true;
		return;
	}
	catch (...)
	{
	}
#endif
	//Then try 3.3 core first
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
	try
	{
		CreateNewWindow();
		m_renderer = std::make_unique<COpenGLRenderer>();
		return;
	}
	catch (...)
	{
	}

	//If it fails, try 3.1 any (3.1 is required for instanced rendering)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

	try
	{
		CreateNewWindow();
		m_renderer = std::make_unique<COpenGLRenderer>();
		return;
	}
	catch (...)
	{
	}
#ifndef RENDERER_NO_LEGACY
	//lastly create legacy 2.0 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	CreateNewWindow();
	m_renderer = std::make_unique<CLegacyGLRenderer>();
#endif
}

CGameWindowGLFW::~CGameWindowGLFW()
{
	glfwTerminate();
	EnableVRMode(false);
}

void CGameWindowGLFW::DoOnDrawScene(std::function<void()> const& handler)
{
	m_onDraw = handler;
}

void CGameWindowGLFW::DoOnResize(std::function<void(int, int)> const& handler)
{
	m_onResize = handler;
	OnReshape(m_window, 600, 600);
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

bool CGameWindowGLFW::EnableVRMode(bool enable, VRViewportFactory const& viewportFactory)
{
	if (enable)
	{
		if (m_vrSystem)
		{
			return true;
		}
		HmdError err;
		m_vrSystem = VR_Init(&err, VRApplication_Scene);
		LogVRError(err);
		if (m_vrSystem)
		{
			uint32_t width, height;
			m_vrSystem->GetRecommendedRenderTargetSize(&width, &height);
			auto compositor = VR_GetGenericInterface(IVRCompositor_Version, &err);
			LogVRError(err);
			if (!compositor)
			{
				VR_Shutdown();
				m_vrSystem = nullptr;
				m_eyeTextures.clear();
				return false;
			}
			if (!viewportFactory)
			{
				return false;
			}
			auto viewports = viewportFactory(width, height);
			m_eyeTextures.push_back(&viewports.first.GetTexture(0));
			m_eyeTextures.push_back(&viewports.second.GetTexture(0));
		}
		else
		{
			return false;
		}
	}
	else
	{
		VR_Shutdown();
		m_eyeTextures.clear();
	}
	return true;
}

view::IInput& CGameWindowGLFW::GetInput()
{
	return *m_input;
}

view::IRenderer& CGameWindowGLFW::GetRenderer()
{
	return *m_renderer;
}

view::IViewHelper& CGameWindowGLFW::GetViewHelper()
{
	return *m_renderer;
}

void CGameWindowGLFW::GetWindowSize(int& width, int& height)
{
	glfwGetWindowSize(m_window, &width, &height);
}

