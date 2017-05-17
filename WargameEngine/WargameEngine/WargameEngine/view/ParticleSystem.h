#pragma once
#include <map>
#include <memory>
#include <vector>
#include "ParticleModel.h"
#include "../model/ParticleEffect.h"

namespace wargameEngine
{
namespace view
{
class IRenderer;
class IShaderProgram;

class ParticleSystem
{
public:
	ParticleSystem(IRenderer & renderer);
	void SetShaders(const Path& vertex, const Path& fragment);
	void Draw(model::ParticleEffect const& particleEffect);
	model::IParticleUpdater* GetParticleUpdater(const Path& path);
private:
	std::map<Path, ParticleModel> m_models;
	IRenderer & m_renderer;
	std::unique_ptr<IShaderProgram> m_shaderProgram;
	std::vector<CVector3f> m_vertexBuffer;
	std::vector<CVector2f> m_texCoordBuffer2;
	std::vector<float> m_texCoordBuffer;
	std::vector<float> m_colorBuffer;
};
}
}