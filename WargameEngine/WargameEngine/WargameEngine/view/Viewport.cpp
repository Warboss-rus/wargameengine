#include "Viewport.h"
#include <stdexcept>
#include "ICamera.h"
#include "IViewHelper.h"

CViewport::CViewport(int x, int y, int width, int height, float fieldOfView, IViewHelper & renderer, bool resize)
	: m_x(x), m_y(y), m_width(width), m_height(height), m_fieldOfView(fieldOfView), m_renderer(renderer), m_resize(resize)
{
}

CViewport::~CViewport()
{
}

ICamera& CViewport::GetCamera()
{
	return *m_camera;
}

ICamera const& CViewport::GetCamera() const
{
	return *m_camera;
}

void CViewport::SetCamera(std::unique_ptr<ICamera> && camera)
{
	m_camera = std::move(camera);
}

void CViewport::Draw(DrawFunc const& draw)
{
	if (fabs(m_polygonOffsetFactor) > FLT_EPSILON && fabs(m_polygonOffsetUnits) > FLT_EPSILON)
	{
		m_renderer.EnablePolygonOffset(true, m_polygonOffsetFactor, m_polygonOffsetUnits);
	}
	m_renderer.SetUpViewport(m_x, m_y, m_width, m_height, m_fieldOfView, m_nearPane, m_farPane);
	m_renderer.ResetViewMatrix();
	m_renderer.LookAt(m_camera->GetPosition(), m_camera->GetDirection(), m_camera->GetUpVector());
	draw(false, true);
}

ICachedTexture const& CViewport::GetTexture() const
{
	throw std::runtime_error("Viewports renders to the screen so there is no texture");
}

Matrix4F CViewport::GetProjectionMatrix() const
{
	Matrix4F result;
	m_renderer.SetUpViewport(m_x, m_y, m_width, m_height, m_fieldOfView, m_nearPane, m_farPane);
	m_renderer.GetProjectionMatrix(result);
	m_renderer.RestoreViewport();
	return result;
}

Matrix4F CViewport::GetViewMatrix() const
{
	Matrix4F result;
	m_renderer.PushMatrix();
	m_renderer.ResetViewMatrix();
	m_renderer.LookAt(m_camera->GetPosition(), m_camera->GetDirection(), m_camera->GetUpVector());
	m_renderer.GetViewMatrix(result);
	m_renderer.PopMatrix();
	return result;
}

void CViewport::Resize(int width, int height)
{
	if (m_resize)
	{
		m_width = width;
		m_height = height;
	}
}

void CViewport::SetPolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	m_polygonOffsetFactor = enable ? factor : 0.0f;
	m_polygonOffsetUnits = enable ? units : 0.0f;
}

void CViewport::SetClippingPlanes(double near /*= 1.0*/, double far /*= 1000.0*/)
{
	m_nearPane = near;
	m_farPane = far;
}
