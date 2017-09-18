#pragma once
#include <unordered_map>
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
class TextureManager;

class ParticleSystem
{
public:
	void SetShaders(const Path& vertex, const Path& fragment, IRenderer& renderer);
	void Draw(model::ParticleEffect const& particleEffect, IRenderer & renderer, TextureManager& textureManager);
	model::IParticleUpdater* GetParticleUpdater(const Path& path);
private:
	std::unordered_map<Path, ParticleModel> m_models;
	std::unique_ptr<IShaderProgram> m_shaderProgram;
	std::vector<CVector3f> m_vertexBuffer;
	std::vector<CVector2f> m_texCoordBuffer2;
	std::vector<float> m_texCoordBuffer;
	std::vector<float> m_colorBuffer;
};
}
}