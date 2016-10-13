#include "ParticleSystem.h"
#include "IRenderer.h"

using namespace std;

CParticleSystem::CParticleSystem(IRenderer & renderer)
	:m_renderer(renderer), m_shaderManager(m_renderer.CreateShaderManager())
{
}

void CParticleSystem::SetShaders(std::wstring const& vertex, std::wstring const& fragment)
{
	if (m_renderer.SupportsFeature(Feature::INSTANSING))
	{
		m_shaderManager->NewProgram(vertex, fragment);
		m_instanced = true;
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

	if (m_instanced)
	{
		m_shaderManager->BindProgram();
		m_positionBuffer.resize(particleEffect.GetParticles().size() * 4);
		if (useTexCoordAttrib) m_texCoordBuffer.resize(particleEffect.GetParticles().size() * 2);
		if (useColorAttrib) m_colorBuffer.resize(particleEffect.GetParticles().size() * 4);
		size_t arrIndex = 0;
		for (auto& particle : particleEffect.GetParticles())
		{
			memcpy(m_positionBuffer.data() + arrIndex * 4, &particle.m_position, sizeof(float) * 3);
			m_positionBuffer[arrIndex * 4 + 3] = particle.m_scale;
			if (useTexCoordAttrib) memcpy(m_texCoordBuffer.data() + arrIndex * 2, &model.GetParticleTexcoords(particle), sizeof(float) * 2);
			if (useColorAttrib) memcpy(m_colorBuffer.data() + arrIndex * 4, particle.m_color, sizeof(float) * 4);
			++arrIndex;
		}
		m_shaderManager->SetPerInstanceVertexAttribute("instancePosition", 4, m_positionBuffer.size() / 4, m_positionBuffer.data());
		if (useTexCoordAttrib)
		{
			m_shaderManager->SetPerInstanceVertexAttribute("instanceTexCoordPos", 2, m_texCoordBuffer.size() / 2, m_texCoordBuffer.data());
		}
		else
		{
			static CVector2f emptyTexCoord(0.0f, 0.0f);
			m_shaderManager->DisableVertexAttribute("instanceTexCoordPos", 2, &emptyTexCoord.x);
		}
		if (useColorAttrib)
		{
			m_shaderManager->SetPerInstanceVertexAttribute("instanceColor", 4, m_colorBuffer.size() / 4, m_colorBuffer.data());
		}
		else
		{
			static float emptyColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			m_renderer.SetColor(emptyColor);
		}

		CVector3f vertex[] = { p0, p1, p3, p1, p3, p2 };
		CVector2f texCoord[] = { t0, t1, t3, t1, t3, t2 };
		auto buffer = m_renderer.CreateVertexBuffer(&vertex->x, nullptr, &texCoord->x, sizeof(vertex) / sizeof(float));
		buffer->Bind();
		buffer->DrawInstanced(6, particleEffect.GetParticles().size());
		buffer->UnBind();

		float def[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_shaderManager->DisableVertexAttribute("instancePosition", 3, def);
		m_shaderManager->DisableVertexAttribute("instanceTexCoordPos", 2, def);
		//m_shaderManager->DisableVertexAttribute("instanceColor", 4, def);
		m_shaderManager->UnBindProgram();
	}
	else
	{
		m_vertexBuffer.resize(particleEffect.GetParticles().size() * 4);
		m_texCoordBuffer2.resize(particleEffect.GetParticles().size() * 4);
		if (useColorAttrib) m_colorBuffer.resize(particleEffect.GetParticles().size() * 4 * 4);
		size_t arrIndex = 0;
		for (auto& particle : particleEffect.GetParticles())
		{
			auto& pos = particle.m_position;
			float scale = particle.m_scale;
			auto tc = model.GetParticleTexcoords(particle);
			m_vertexBuffer.insert(m_vertexBuffer.end(), { p0 * scale + pos, p1 * scale + pos, p2 * scale + pos, p3 * scale + pos });
			if (useTexCoordAttrib) m_texCoordBuffer2.insert(m_texCoordBuffer2.end(), { t0 + tc, t1 + tc, t2 + tc, t3 + tc });
			if (useColorAttrib) 
				for(int i = 0; i < 6; ++i)
					memcpy(m_colorBuffer.data() + arrIndex * 24 + i * 4, particle.m_color, sizeof(float) * 4);
			++arrIndex;
		}
		if(useColorAttrib) m_shaderManager->SetVertexAttribute("color", 4, m_colorBuffer.size() / 4, m_colorBuffer.data());
		m_renderer.RenderArrays(RenderMode::RECTANGLES, m_vertexBuffer, {}, m_texCoordBuffer2);
	}
	m_renderer.SetTexture(L"");
	m_renderer.PopMatrix();
}

IParticleUpdater* CParticleSystem::GetParticleUpdater(std::wstring const& path)
{
	if (m_models.find(path) == m_models.end())
	{
		m_models.emplace(path, CParticleModel(path));
	}
	return &m_models.at(path);
}