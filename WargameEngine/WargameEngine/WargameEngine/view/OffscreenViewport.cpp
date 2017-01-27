#include "OffscreenViewport.h"
#include <float.h>

COffscreenViewport::COffscreenViewport(CachedTextureType type, int width, int height, float fieldOfView, IViewHelper & viewHelper, int textureIndex)
	: CViewportBase(0, 0, width, height, fieldOfView, viewHelper), m_depthOnly(type == CachedTextureType::DEPTH)
{
	m_texture = m_renderer.CreateTexture(NULL, width, height, type);
	m_FBO = m_renderer.CreateFramebuffer();
	m_FBO->Bind();
	m_FBO->AssignTexture(*m_texture, type);
	m_FBO->UnBind();
	if (textureIndex != -1)
	{
		m_renderer.ActivateTextureSlot(static_cast<TextureSlot>(textureIndex));
	}
	m_texture->Bind();
	m_renderer.ActivateTextureSlot(TextureSlot::eDiffuse);
}

void COffscreenViewport::Draw(DrawFunc const& draw)
{
	m_FBO->Bind();
	m_renderer.ClearBuffers(!m_depthOnly, true);
	if (fabs(m_polygonOffsetFactor) > FLT_EPSILON && fabs(m_polygonOffsetUnits) > FLT_EPSILON)
	{
		m_renderer.EnablePolygonOffset(true, m_polygonOffsetFactor, m_polygonOffsetUnits);
	}
	m_renderer.SetUpViewport(0, 0, m_width, m_height, m_fieldOfView, m_nearPane, m_farPane);
	m_renderer.LookAt(m_camera->GetPosition(), m_camera->GetDirection(), m_camera->GetUpVector());

	m_renderer.GetViewMatrix(m_viewMatrix);
	m_renderer.GetProjectionMatrix(m_projectionMatrix);

	draw(m_depthOnly, false);
	m_renderer.EnablePolygonOffset(false);
	m_FBO->UnBind();
}

ICachedTexture const& COffscreenViewport::GetTexture() const
{
	return *m_texture;
}

bool COffscreenViewport::PointIsInViewport(int, int) const
{
	return false;//offscreen viewport is not shown on a window
}
