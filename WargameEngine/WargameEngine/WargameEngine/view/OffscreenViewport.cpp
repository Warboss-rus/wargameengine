#include "OffscreenViewport.h"

COffscreenViewport::COffscreenViewport(CachedTextureType type, int width, int height, float fieldOfView, IViewHelper & viewHelper, int textureIndex)
	: m_depthOnly(type == CachedTextureType::DEPTH), m_width(width), m_height(height), m_fieldOfView(fieldOfView), m_viewHelper(viewHelper), m_textureIndex(textureIndex)
{
	m_texture = m_viewHelper.CreateTexture(NULL, width, height, type);
	m_FBO = m_viewHelper.CreateFramebuffer();
	m_FBO->Bind();
	m_FBO->AssignTexture(*m_texture, type);
	m_FBO->UnBind();
}

ICamera& COffscreenViewport::GetCamera()
{
	return *m_camera;
}

ICamera const& COffscreenViewport::GetCamera() const
{
	return *m_camera;
}

void COffscreenViewport::SetCamera(std::unique_ptr<ICamera> && camera)
{
	m_camera = std::move(camera);
}

void COffscreenViewport::Draw(DrawFunc const& draw)
{
	if (fabs(m_polygonOffsetFactor) > FLT_EPSILON && fabs(m_polygonOffsetUnits) > FLT_EPSILON)
	{
		m_viewHelper.EnablePolygonOffset(true, m_polygonOffsetFactor, m_polygonOffsetUnits);
	}
	m_FBO->Bind();

	m_viewHelper.ClearBuffers(!m_depthOnly, m_depthOnly);
	m_viewHelper.SetUpViewport(0, 0, m_width, m_height, m_fieldOfView, m_nearPane, m_farPane);
	m_viewHelper.ResetViewMatrix();
	m_viewHelper.LookAt(m_camera->GetPosition(), m_camera->GetDirection(), m_camera->GetUpVector());

	m_viewHelper.GetViewMatrix(m_viewMatrix);
	m_viewHelper.GetProjectionMatrix(m_projectionMatrix);

	draw(m_depthOnly, false);
	m_FBO->UnBind();

	if (m_textureIndex != -1)
	{
		m_viewHelper.ActivateTextureSlot(static_cast<TextureSlot>(m_textureIndex));
		m_texture->Bind();
		m_viewHelper.ActivateTextureSlot(TextureSlot::eDiffuse);
	}
	m_viewHelper.EnablePolygonOffset(false);
}

ICachedTexture const& COffscreenViewport::GetTexture() const
{
	return *m_texture;
}

Matrix4F COffscreenViewport::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

Matrix4F COffscreenViewport::GetViewMatrix() const
{
	return m_viewMatrix;
}

void COffscreenViewport::Resize(int, int)
{
	//do nothing, offscreen viewports are not affected by resize
}

void COffscreenViewport::SetPolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	m_polygonOffsetFactor = enable ? factor : 0.0f;
	m_polygonOffsetUnits = enable ? units : 0.0f;
}

void COffscreenViewport::SetClippingPlanes(double near /*= 1.0*/, double far /*= 1000.0*/)
{
	m_nearPane = near;
	m_farPane = far;
}
