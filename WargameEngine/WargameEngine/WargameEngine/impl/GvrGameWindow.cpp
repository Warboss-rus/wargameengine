#include "GvrGameWindow.h"

static gvr::Mat4f MatrixMul(const gvr::Mat4f& matrix1,
	const gvr::Mat4f& matrix2) {
	gvr::Mat4f result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += matrix1.m[i][k] * matrix2.m[k][j];
			}
		}
	}
	return result;
}

CGvrGameWindow::CGvrGameWindow(gvr_context* gvr_context)
	: m_gvr_api(gvr::GvrApi::WrapNonOwned(gvr_context))
	, m_viewport_left(m_gvr_api->CreateBufferViewport())
	, m_viewport_right(m_gvr_api->CreateBufferViewport())
{
}

void CGvrGameWindow::Init()
{
	m_gvr_api->InitializeGl();

	std::vector<gvr::BufferSpec> specs;
	specs.emplace_back(m_gvr_api->CreateBufferSpec());
	m_swapchain = m_gvr_api->CreateSwapChain(specs);
	m_viewport_list = m_gvr_api->CreateEmptyBufferViewportList();
}

void CGvrGameWindow::Draw()
{
	m_viewport_list.SetToRecommendedBufferViewports();
	gvr::Frame frame = m_swapchain.AcquireFrame();
	gvr::Mat4f head_matrix = m_gvr_api->GetHeadSpaceFromStartSpaceRotation(m_gvr_api->GetTimePointNow());
	gvr::Mat4f left_eye_matrix = MatrixMul(m_gvr_api->GetEyeFromHeadMatrix(GVR_LEFT_EYE), head_matrix);
	gvr::Mat4f right_eye_matrix = MatrixMul(m_gvr_api->GetEyeFromHeadMatrix(GVR_RIGHT_EYE), head_matrix);

	frame.BindBuffer(0);
	m_viewport_list.GetBufferViewport(GVR_LEFT_EYE, &m_viewport_left);
	m_onDraw();
	m_viewport_list.GetBufferViewport(GVR_RIGHT_EYE, &m_viewport_right);
	m_onDraw();
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

bool CGvrGameWindow::EnableVRMode(bool show, VRViewportFactory const&)
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
	width = 1920;
	height = 1080;
}