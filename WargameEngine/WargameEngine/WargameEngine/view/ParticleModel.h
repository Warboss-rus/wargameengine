#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../model/Particle.h"

class CParticleModel : public IParticleUpdater
{
public:
	CParticleModel(std::wstring const& file);
	std::wstring GetTexture() const;
	CVector2f GetTextureFrameSize() const;
	CVector2f GetParticleSize() const;
	bool HasDifferentTexCoords() const;
	bool HasDifferentColors() const;
	
//IParticleUpdater
	virtual float GetAverageLifeTime() const override;
	virtual void InitParticle(sParticle & particle) const override;
	virtual void UpdateParticles(std::vector<sParticle> & particles) const override;

	class IEmitter
	{
	public:
		virtual ~IEmitter() {}
		virtual void InitParticle(sParticle & particle) = 0;
	};
	class IUpdater
	{
	public:
		virtual ~IUpdater() {}
		virtual void Update(std::vector<sParticle> & particles) = 0;
	};
private:
	std::unique_ptr<IEmitter> m_emitter;
	std::unique_ptr<IUpdater> m_updater;
	std::wstring m_texture;
	float m_minLifeTime;
	float m_maxLifeTime;
	float m_minScale;
	float m_maxScale;
	CVector2f m_particleSize;
	CVector2f m_textureFrameSize;
};