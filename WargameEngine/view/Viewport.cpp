#include "Viewport.h"
#include "IShaderManager.h"
#include "IViewHelper.h"
#include <stdexcept>

namespace wargameEngine
{
namespace view
{
Viewport::Viewport(int x, int y, int width, int height, float fieldOfView, IViewHelper& renderer, IInput& input, bool onScreen, bool resize)
	: m_camera(input)
	, m_x(x)
	, m_y(y)
	, m_width(width)
	, m_height(height)
	, m_fieldOfView(fieldOfView)
	, m_renderer(&renderer)
	, m_depthOnly(!onScreen)
	, m_resize(resize)
{
	Matrix4F emptyMatrix;
	memcpy(m_viewMatrix, emptyMatrix, sizeof(float) * 16);
	memcpy(m_projectionMatrix, emptyMatrix, sizeof(float) * 16);
	if (!onScreen)
	{
		m_FBO = m_renderer->CreateFramebuffer();
	}
}

Viewport::~Viewport()
{
}

Camera& Viewport::GetCamera()
{
	return m_camera;
}

Camera const& Viewport::GetCamera() const
{
	return m_camera;
}

void Viewport::AttachNewTexture(IRenderer::CachedTextureType type, int textureIndex /*= -1*/)
{
	if (!m_FBO)
	{
		throw std::runtime_error("Cannot attach texture to onscreen viewport");
	}

	auto texture = m_renderer->CreateTexture(NULL, m_width, m_height, type);
	m_FBO->Bind();
	m_FBO->AssignTexture(*texture, type);
	m_FBO->UnBind();
	m_renderer->SetTexture(*texture, static_cast<IRenderer::TextureSlot>(textureIndex));
	m_textures.push_back(std::move(texture));
}

void Viewport::Bind()
{
	if (m_FBO)
	{
		m_FBO->Bind();
		m_renderer->ClearBuffers(!m_depthOnly, true);
	}
	if (fabs(m_polygonOffsetFactor) > FLT_EPSILON && fabs(m_polygonOffsetUnits) > FLT_EPSILON)
	{
		m_renderer->EnablePolygonOffset(true, m_polygonOffsetFactor, m_polygonOffsetUnits);
	}
	else
	{
		m_renderer->EnablePolygonOffset(false);
	}
	m_renderer->SetUpViewport(0, 0, m_width, m_height, m_fieldOfView, m_nearPane, m_farPane);
	m_renderer->LookAt(m_camera.GetPosition(), m_camera.GetDirection(), m_camera.GetUpVector());

	m_viewMatrix = m_renderer->GetViewMatrix();
	m_projectionMatrix = m_renderer->GetProjectionMatrix();
}

void Viewport::Unbind()
{
	if (m_FBO)
	{
		m_FBO->UnBind();
	}
}

bool Viewport::IsDepthOnly() const
{
	return m_depthOnly;
}

bool Viewport::DrawUI() const
{
	return !m_FBO;
}

ICachedTexture const& Viewport::GetTexture(size_t index) const
{
	return *m_textures[index];
}

IOcclusionQuery& Viewport::GetOcclusionQuery(const model::IBaseObject* object)
{
	auto it = m_occlusionQueries.find(object);
	if (it == m_occlusionQueries.end())
	{
		it = m_occlusionQueries.emplace(std::make_pair(object, m_renderer->CreateOcclusionQuery())).first;
	}
	return *it->second;
}

const float* Viewport::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

const float* Viewport::GetViewMatrix() const
{
	return m_viewMatrix;
}

void Viewport::SetPolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	m_polygonOffsetFactor = enable ? factor : 0.0f;
	m_polygonOffsetUnits = enable ? units : 0.0f;
}

void Viewport::SetClippingPlanes(float near /*= 1.0f*/, float far /*= 1000.0f*/)
{
	m_nearPane = near;
	m_farPane = far;
}

bool Viewport::PointIsInViewport(int x, int y) const
{
	return !m_FBO && x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height;
}

void Viewport::Resize(int width, int height)
{
	if (m_resize)
	{
		m_width = width;
		m_height = height;
	}
}

void Viewport::SetUpShadowMap() const
{
	if (m_shadowMapViewport)
	{
		Matrix4F cameraModelViewMatrix = m_renderer->GetViewMatrix();

		Matrix4F cameraInverseModelViewMatrix = cameraModelViewMatrix.Invert();

		Matrix4F lightMatrix;
		lightMatrix.Scale(0.5f);
		lightMatrix.Translate(0.5, 0.5, 0.5);
		lightMatrix *= m_shadowMapViewport->GetProjectionMatrix();
		lightMatrix *= m_shadowMapViewport->GetViewMatrix();
		lightMatrix *= cameraInverseModelViewMatrix;

		m_renderer->GetShaderManager().SetUniformValue("lightMatrix", 16, 1, lightMatrix);
	}
}

void Viewport::SetShaders(const Path& vertexShader /*= Path()*/, const Path& fragmentShader /*= Path()*/, const Path& geometryShader /*= Path()*/)
{
	m_shaderProgram = m_renderer->GetShaderManager().NewProgram(vertexShader, fragmentShader, geometryShader);
}

}
}