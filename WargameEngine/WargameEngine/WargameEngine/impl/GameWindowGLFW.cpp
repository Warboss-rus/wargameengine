#include "GameWindowGLFW.h"
#define RENDERER_NO_VULKAN
#define RENDERER_NO_LEGACY
#ifndef RENDERER_NO_VULKAN
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
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
#define _USE_MATH_DEFINES
#include <math.h>
#include "../view/Matrix4.h"
#ifndef RENDERER_NO_LEGACY
#include "LegacyOpenGLRenderer.h"
#endif

using namespace vr;

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

void GetRotation(Matrix4F const& matrix, double& Yaw, double& Pitch, double& Roll)
{
	if (matrix.m_union._11 == 1.0f)
	{
		Yaw = atan2f(matrix.m_union._13, matrix.m_union._34);
		Pitch = 0;
		Roll = 0;

	}
	else if (matrix.m_union._11 == -1.0f)
	{
		Yaw = atan2f(matrix.m_union._13, matrix.m_union._34);
		Pitch = 0;
		Roll = 0;
	}
	else
	{

		Yaw = atan2(-matrix.m_union._31, matrix.m_union._11);
		Pitch = asin(matrix.m_union._21);
		Roll = atan2(-matrix.m_union._23, matrix.m_union._22);
	}
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
						double x, y, z;
						GetRotation(matrix, z, y, x);
						m_input->SetHeadRotation(i, static_cast<float>(-x * 180 / M_PI), static_cast<float>(y * 180 / M_PI), static_cast<float>(z * 180 / M_PI));
					}
				}
			}
			m_input->UpdateControllers();
#ifndef RENDERER_NO_VULKAN
			if (m_vulkanRenderer)
			{
				auto* renderer = reinterpret_cast<CVulkanRenderer*>(m_renderer.get());
				renderer->AcquireImage();
				renderer->ClearBuffers();
				renderer->SetUpViewport(0, 0, 600, 600, 65.0f);
				renderer->DrawIn2D([&renderer] {
					renderer->SetTexture(L"..\\Killteam\\texture\\sand.bmp", true);
					renderer->RenderArrays(RenderMode::TRIANGLES, {
						{0.0f, 0.0f, 0.0f},
						{300.0f, 0.0f, 0.0f},
						{0.0f, 300.0f, 0.0f}
					}, {}, {
						{0.0f, 0.0f},
						{1.0f, 0.0f},
						{0.0f, 1.0f}
					});
				});
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
				for (int i = 0; i < 2; ++i)
				{
					unsigned int t = reinterpret_cast<const COpenGlCachedTexture&>(*m_eyeTextures[i]);
					const vr::Texture_t tex = { reinterpret_cast<void*>(t), TextureType_OpenGL, ColorSpace_Gamma };
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
		const std::vector<char*> instanceExtensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
		};
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

void CGameWindowGLFW::GetWindowSize(int& width, int& height)
{
	glfwGetWindowSize(m_window, &width, &height);
}

