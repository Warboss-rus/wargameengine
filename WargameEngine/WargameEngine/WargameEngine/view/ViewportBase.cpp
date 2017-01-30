#include "ViewportBase.h"
#include <stdexcept>
#include "ICamera.h"
#include "IViewHelper.h"
#include "Matrix4.h"

CViewportBase::CViewportBase(int x, int y, int width, int height, float fieldOfView, IViewHelper & renderer)
	: m_x(x), m_y(y), m_width(width), m_height(height), m_fieldOfView(fieldOfView), m_renderer(renderer)
{
}

CViewportBase::~CViewportBase()
{
}

ICamera& CViewportBase::GetCamera()
{
	return *m_camera;
}

ICamera const& CViewportBase::GetCamera() const
{
	return *m_camera;
}

void CViewportBase::SetCamera(std::unique_ptr<ICamera> && camera)
{
	m_camera = std::move(camera);
}

ICachedTexture const& CViewportBase::GetTexture() const
{
	throw std::runtime_error("Viewport renders to the screen so there is no texture");
}

IOcclusionQuery & CViewportBase::GetOcclusionQuery(const IBaseObject* object)
{
	auto it = m_occlusionQueries.find(object);
	if (it == m_occlusionQueries.end())
	{
		it = m_occlusionQueries.emplace(std::make_pair(object, m_renderer.CreateOcclusionQuery())).first;
	}
	return *it->second;
}

const float* CViewportBase::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

const float* CViewportBase::GetViewMatrix() const
{
	return m_viewMatrix;
}

void CViewportBase::SetPolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	m_polygonOffsetFactor = enable ? factor : 0.0f;
	m_polygonOffsetUnits = enable ? units : 0.0f;
}

void CViewportBase::SetClippingPlanes(float near /*= 1.0f*/, float far /*= 1000.0f*/)
{
	m_nearPane = near;
	m_farPane = far;
}

bool CViewportBase::PointIsInViewport(int x, int y) const
{
	return x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height;
}

void CViewportBase::Resize(int, int)
{
	//do nothing
}

void CViewportBase::SetUpShadowMap() const
{
	if (m_shadowMapViewport)
	{
		Matrix4F cameraModelViewMatrix;
		m_renderer.GetViewMatrix(cameraModelViewMatrix);

		Matrix4F cameraInverseModelViewMatrix = cameraModelViewMatrix.Invert();

		Matrix4F lightMatrix;
		lightMatrix.Scale(0.5f);
		lightMatrix.Translate(0.5, 0.5, 0.5);
		lightMatrix *= m_shadowMapViewport->GetProjectionMatrix();
		lightMatrix *= m_shadowMapViewport->GetViewMatrix();
		lightMatrix *= cameraInverseModelViewMatrix;

		m_renderer.GetShaderManager().SetUniformValue("lightMatrix", 16, 1, lightMatrix);
	}
}
