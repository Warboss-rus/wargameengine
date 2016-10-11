#pragma once
#include <map>
#include <memory>
#include <vector>
#include "ParticleModel.h"
#include "..\model\Particle.h"

class IRenderer;
class IShaderManager;

class CParticleSystem
{
public:
	CParticleSystem(IRenderer & renderer);
	void SetShaders(std::wstring const& vertex, std::wstring const& fragment);
	void Draw(CParticleEffect const& particleEffect);
	IParticleUpdater* GetParticleUpdater(std::wstring const& path);
private:
	bool m_instanced = false;
	std::map<std::wstring, CParticleModel> m_models;
	IRenderer & m_renderer;
	std::unique_ptr<IShaderManager> m_shaderManager;
	std::vector<CVector3f> m_vertexBuffer;
	std::vector<CVector2f> m_texCoordBuffer2;
	std::vector<float> m_texCoordBuffer;
	std::vector<float> m_positionBuffer;
	std::vector<float> m_colorBuffer;
};