#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../model/ParticleEffect.h"

namespace wargameEngine
{
namespace view
{
class ParticleModel : public model::IParticleUpdater
{
public:
	ParticleModel(const Path& file);
	Path GetTexture() const;
	CVector2f GetTextureFrameSize() const;
	CVector2f GetParticleSize() const;
	bool HasDifferentTexCoords() const;
	bool HasDifferentColors() const;

	//IParticleUpdater
	virtual float GetAverageLifeTime() const override;
	virtual void InitParticle(model::Particle & particle) const override;
	virtual void UpdateParticles(std::vector<model::Particle> & particles) const override;

	class IEmitter
	{
	public:
		virtual ~IEmitter() {}
		virtual void InitParticle(model::Particle & particle) = 0;
	};
	class IUpdater
	{
	public:
		virtual ~IUpdater() {}
		virtual void Update(std::vector<model::Particle> & particles) = 0;
	};
private:
	std::unique_ptr<IEmitter> m_emitter;
	std::unique_ptr<IUpdater> m_updater;
	Path m_texture;
	float m_minLifeTime;
	float m_maxLifeTime;
	float m_minScale;
	float m_maxScale;
	CVector2f m_particleSize;
	CVector2f m_textureFrameSize;
};
}
}