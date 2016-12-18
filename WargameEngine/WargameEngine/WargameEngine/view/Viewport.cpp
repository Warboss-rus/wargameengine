#include "Viewport.h"
#include "ICamera.h"
#include "IViewHelper.h"

CViewport::CViewport(int x, int y, int width, int height, float fieldOfView, IViewHelper & renderer, bool resize)
	: CViewportBase(x, y, width, height, fieldOfView, renderer), m_resize(resize)
{
}

void CViewport::Draw(DrawFunc const& draw)
{
	if (fabs(m_polygonOffsetFactor) > FLT_EPSILON && fabs(m_polygonOffsetUnits) > FLT_EPSILON)
	{
		m_renderer.EnablePolygonOffset(true, m_polygonOffsetFactor, m_polygonOffsetUnits);
	}
	m_renderer.SetUpViewport(m_x, m_y, m_width, m_height, m_fieldOfView, m_nearPane, m_farPane);
	m_renderer.LookAt(m_camera->GetPosition(), m_camera->GetDirection(), m_camera->GetUpVector());
	m_renderer.GetViewMatrix(m_viewMatrix);
	m_renderer.GetProjectionMatrix(m_projectionMatrix);
	draw(false, true);
}

void CViewport::Resize(int width, int height)
{
	if (m_resize)
	{
		m_width = width;
		m_height = height;
	}
}
