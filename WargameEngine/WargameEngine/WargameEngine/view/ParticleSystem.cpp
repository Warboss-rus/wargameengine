#include "ParticleSystem.h"
#include "IRenderer.h"
#include "IShaderManager.h"

using namespace std;

CParticleSystem::CParticleSystem(IRenderer & renderer)
	:m_renderer(renderer)
{
}

void CParticleSystem::SetShaders(const Path& vertex, const Path& fragment)
{
	if (m_renderer.SupportsFeature(Feature::INSTANSING))
	{
		m_shaderProgram = m_renderer.GetShaderManager().NewProgram(vertex, fragment);
	}
}

void CParticleSystem::Draw(CParticleEffect const& particleEffect)
{
	float modelview[4][4];
	m_renderer.GetViewMatrix(&modelview[0][0]);
	m_renderer.PushMatrix();
	CVector3f const & coords = particleEffect.GetPosition();
	m_renderer.Translate(coords.x, coords.y, coords.z);
	m_renderer.Scale(particleEffect.GetScale());
	auto& model = m_models.at(particleEffect.GetEffectPath());
	m_renderer.SetTexture(model.GetTexture());

	auto size = model.GetParticleSize();
	auto textureFrameSize = CVector2f(1.0f, 1.0f) / model.GetTextureFrameSize();
	float sizeX2 = size.x * 0.5f;
	float sizeY2 = size.y * 0.5f;
	CVector3f xAxis(modelview[0][0] * sizeX2, modelview[1][0] * sizeX2, modelview[2][0] * sizeX2);
	CVector3f yAxis(modelview[0][1] * sizeY2, modelview[1][1] * sizeY2, modelview[2][1] * sizeY2);
	CVector3f zAxis(modelview[0][2], modelview[1][2], modelview[2][2]);
	CVector3f p0(-xAxis.x + yAxis.x, -xAxis.y + yAxis.y, -xAxis.z + yAxis.z);
	CVector3f p1(-xAxis.x - yAxis.x, -xAxis.y - yAxis.y, -xAxis.z - yAxis.z);
	CVector3f p2(+xAxis.x - yAxis.x, +xAxis.y - yAxis.y, +xAxis.z - yAxis.z);
	CVector3f p3(+xAxis.x + yAxis.x, +xAxis.y + yAxis.y, +xAxis.z + yAxis.z);
	CVector2f t0(0.0f, 0.0f);
	CVector2f t1(0.0f, textureFrameSize.y);
	CVector2f t2(textureFrameSize.x, textureFrameSize.y);
	CVector2f t3(textureFrameSize.x, 0.0f);

	bool useTexCoordAttrib = model.HasDifferentTexCoords();
	bool useColorAttrib = model.HasDifferentColors();
	auto& shaderManager = m_renderer.GetShaderManager();
	auto& particles = particleEffect.GetParticles();
	size_t particlesCount = particles.size();

	if (m_shaderProgram)
	{
		shaderManager.PushProgram(*m_shaderProgram);
		CVector3f vertex[] = { p0, p1, p3, p1, p3, p2 };
		CVector2f texCoord[] = { t0, t1, t3, t1, t3, t2 };
		auto buffer = m_renderer.CreateVertexBuffer(reinterpret_cast<float*>(vertex), nullptr, reinterpret_cast<float*>(texCoord), 6, true);
		buffer->Bind();
		shaderManager.SetVertexAttribute("instancePosition", 4, particlesCount, particleEffect.GetPositionCache().data(), true);
		if (useTexCoordAttrib)
		{
			shaderManager.SetVertexAttribute("instanceTexCoordPos", 2, particlesCount, particleEffect.GetTexCoordCache().data(), true);
		}
		if (useColorAttrib)
		{
			shaderManager.SetVertexAttribute("instanceColor", 4, particlesCount, particleEffect.GetColorCache().data(), true);
		}

		buffer->DrawInstanced(6, particlesCount);
		static float empty[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		shaderManager.DisableVertexAttribute("instanceColor", 4, empty);
		shaderManager.DisableVertexAttribute("instancePosition", 4, empty);
		shaderManager.DisableVertexAttribute("instanceTexCoordPos", 2, empty);
		buffer->UnBind();
		shaderManager.PopProgram();
	}
	else
	{
		m_vertexBuffer.resize(particlesCount * 6);
		m_texCoordBuffer2.resize(particlesCount * 6);
		if (useColorAttrib) m_colorBuffer.resize(particlesCount * 4 * 6);
		size_t arrIndex = 0;
		for (auto& particle : particles)
		{
			CVector3f pos(particle.m_position);
			float scale = *particle.m_scale;
			CVector2f tc(particle.m_texCoord);
			m_vertexBuffer.insert(m_vertexBuffer.end(), { p0 * scale + pos, p1 * scale + pos, p2 * scale + pos, p1 * scale + pos, p2 * scale + pos, p3 * scale + pos });
			if (useTexCoordAttrib) m_texCoordBuffer2.insert(m_texCoordBuffer2.end(), { t0 + tc, t1 + tc, t2 + tc, t1 + tc, t2 + tc, t3 + tc });
			if (useColorAttrib) 
				for(int i = 0; i < 6; ++i)
					memcpy(m_colorBuffer.data() + arrIndex * 24 + i * 4, particle.m_color, sizeof(float) * 4);
			++arrIndex;
		}
		if(useColorAttrib) shaderManager.SetVertexAttribute("color", 4, m_colorBuffer.size() / 4, m_colorBuffer.data());
		m_renderer.RenderArrays(RenderMode::TRIANGLES, m_vertexBuffer, {}, m_texCoordBuffer2);
	}
	m_renderer.UnbindTexture();
	m_renderer.PopMatrix();
}

IParticleUpdater* CParticleSystem::GetParticleUpdater(const Path& path)
{
	if (m_models.find(path) == m_models.end())
	{
		m_models.emplace(path, CParticleModel(path));
	}
	return &m_models.at(path);
}