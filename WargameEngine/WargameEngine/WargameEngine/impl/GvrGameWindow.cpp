#include "GvrGameWindow.h"
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

static const std::string defaultMultiviewVertexShader = "\
#version 300 es\n\
#extension GL_OVR_multiview2 : enable\n\
layout(num_views=2) in vec4 Position;\n\
layout(num_views=2) in vec3 Normal;\n\
layout(num_views=2) in vec2 TexCoord;\n\
uniform mat4 mvp_matrix[2];\n\
out vec2 v_texcoord;\n\
void main()\n\
{\n\
	gl_Position = mvp_matrix[gl_ViewID_OVR] * Position;\n\
	v_texcoord = TexCoord;\n\
}";
static const std::string defaultMultiviewFragmentShader = "\
#version 300 es\n\
precision mediump float;\
uniform vec4 color;\n\
uniform sampler2D mainTexture;\n\
in vec2 v_texcoord;\n\
out vec4 fragColor;\n\
void main()\n\
{\n\
	vec4 tex = texture(mainTexture, v_texcoord);\n\
	fragColor = vec4(tex.xyz + color.xyz, tex.w * color.w);\n\
}";

CGvrGameWindow::CGvrGameWindow(gvr_context* gvr_context)
	: m_gvr_api(gvr::GvrApi::WrapNonOwned(gvr_context))
	, m_viewport_left(m_gvr_api->CreateBufferViewport())
	, m_viewport_right(m_gvr_api->CreateBufferViewport())
{
}

void CGvrGameWindow::Init()
{
	m_gvr_api->InitializeGl();

	m_multiview = m_gvr_api->IsFeatureSupported(GVR_FEATURE_MULTIVIEW);

	std::vector<gvr::BufferSpec> specs;
	specs.emplace_back(m_gvr_api->CreateBufferSpec());
	if (m_multiview)
	{
		specs[0].SetMultiviewLayers(2);
		auto size = specs[0].GetSize();
		specs[0].SetSize({ size.width / 2, size.height });
	}
	specs[0].SetColorFormat(GVR_COLOR_FORMAT_RGBA_8888);
	specs[0].SetDepthStencilFormat(GVR_DEPTH_STENCIL_FORMAT_DEPTH_24);
	m_swapchain = m_gvr_api->CreateSwapChain(specs);
	auto size = m_swapchain.GetBufferSize(0);
	m_renderer.Init(size.width, size.height);
	if(m_onResize) m_onResize(size.width, size.height);
	m_viewport_list = m_gvr_api->CreateEmptyBufferViewportList();
	m_viewport_left = m_gvr_api->CreateBufferViewport();
	m_viewport_right = m_gvr_api->CreateBufferViewport();
	if (m_multiview)
	{
		m_multiviewDefaultProgram = m_renderer.GetShaderManager().NewProgramSource(defaultMultiviewVertexShader, defaultMultiviewFragmentShader);
		m_renderer.GetShaderManager().PushProgram(*m_multiviewDefaultProgram);
	}
}

void CGvrGameWindow::Draw()
{
	m_viewport_list.SetToRecommendedBufferViewports();
	if (m_multiview)
	{
		gvr::BufferViewport* viewport[2] = {&m_viewport_left,&m_viewport_right,};
		for (int eye = GVR_LEFT_EYE; eye < GVR_NUM_EYES; ++eye)
		{
			m_viewport_list.GetBufferViewport(eye, viewport[eye]);
			viewport[eye]->SetSourceUv({ 0, 1, 0, 1 });
			viewport[eye]->SetSourceLayer(eye);
			m_viewport_list.SetBufferViewport(eye, *viewport[eye]);
		}
	}
	gvr::Frame frame = m_swapchain.AcquireFrame();
	gvr::Mat4f head_matrix = m_gvr_api->GetHeadSpaceFromStartSpaceRotation(m_gvr_api->GetTimePointNow());
	glm::mat4 left_eye_matrix = glm::make_mat4(m_gvr_api->GetEyeFromHeadMatrix(GVR_LEFT_EYE).m[0]) * glm::make_mat4(head_matrix.m[0]);
	glm::mat4 right_eye_matrix = glm::make_mat4(m_gvr_api->GetEyeFromHeadMatrix(GVR_RIGHT_EYE).m[0]) * glm::make_mat4(head_matrix.m[0]);

	frame.BindBuffer(0);
	m_renderer.ClearBuffers();
	if (m_multiview)
	{
		m_renderer.SetVrViewMatrices({ glm::value_ptr(glm::transpose(left_eye_matrix)), glm::value_ptr(glm::transpose(right_eye_matrix)) });
		m_onDraw();
	}
	else
	{
		m_viewport_list.GetBufferViewport(GVR_LEFT_EYE, &m_viewport_left);
		auto vrRect = m_viewport_left.GetSourceUv();
		m_renderer.SetVrViewport(vrRect.left, vrRect.bottom, vrRect.right - vrRect.left, vrRect.top - vrRect.bottom, 0.0f);
		m_renderer.SetVrViewMatrices({ glm::value_ptr(glm::transpose(left_eye_matrix)) });
		m_onDraw();
		m_viewport_list.GetBufferViewport(GVR_RIGHT_EYE, &m_viewport_right);
		vrRect = m_viewport_right.GetSourceUv();
		m_renderer.SetVrViewport(vrRect.left, vrRect.bottom, vrRect.right - vrRect.left, vrRect.top - vrRect.bottom, 0.0f);
		m_renderer.SetVrViewMatrices({ glm::value_ptr(glm::transpose(right_eye_matrix)) });
		m_onDraw();
	}
	frame.Unbind();

	// Submit the frame for distortion rendering.
	frame.Submit(m_viewport_list, head_matrix);
}

void CGvrGameWindow::TriggerEvent()
{
}

void CGvrGameWindow::Pause()
{
	m_gvr_api->PauseTracking();
}

void CGvrGameWindow::Resume()
{
	m_gvr_api->ResumeTracking();
}

CInputAndroid& CGvrGameWindow::GetInput()
{
	return m_input;
}

void CGvrGameWindow::LaunchMainLoop()
{
	
}

void CGvrGameWindow::DoOnDrawScene(std::function<void() > const& handler)
{
	m_onDraw = handler;
}

void CGvrGameWindow::DoOnResize(std::function<void(int, int) > const& handler)
{
	m_onResize = handler;
	int width, height;
	GetWindowSize(width, height);
	m_onResize(width, height);
}

void CGvrGameWindow::DoOnShutdown(std::function<void() > const& handler)
{
	m_onShutdown = handler;
}

void CGvrGameWindow::ResizeWindow(int, int)
{
}

void CGvrGameWindow::SetTitle(std::wstring const&)
{
}

void CGvrGameWindow::ToggleFullscreen()
{
}

bool CGvrGameWindow::EnableVRMode(bool, VRViewportFactory const&)
{

}

IInput& CGvrGameWindow::ResetInput()
{
	m_input = CInputAndroid();
	return m_input;
}

IRenderer& CGvrGameWindow::GetRenderer()
{
	return m_renderer;
}

IViewHelper& CGvrGameWindow::GetViewHelper()
{
	return m_renderer;
}

void CGvrGameWindow::EnableMultisampling(bool enable, int level /*= 1.0f*/)
{

}

void CGvrGameWindow::GetWindowSize(int& width, int& height)
{
	if(m_swapchain)
	{ 
		auto size = m_swapchain.GetBufferSize(0);
		width = size.width;
		height = size.height;
	}
	else
	{
		width = 1920;
		height = 1080;
	}
}