#pragma once
#include <memory>
#include <vector>
#include <string>
#include <map>
#include "IGameModel.h"
#include "ObjectInterface.h"
#include "Projectile.h"
#include "Particle.h"
#include "Landscape.h"

class CGameModel : public IGameModel
{
public:
	CGameModel() = default;
	virtual size_t GetObjectCount() const override;
	void Clear();
	std::shared_ptr<const IObject> Get3DObject(size_t number) const;
	virtual std::shared_ptr<IObject> Get3DObject(size_t number) override;
	virtual std::shared_ptr<IObject> Get3DObject(const IBaseObject * obj) override;
	virtual void AddObject(std::shared_ptr<IObject> pObject) override;
	virtual void DeleteObjectByPtr(std::shared_ptr<IObject> pObject) override;
	void SelectObject(std::shared_ptr<IObject> pObject);
	std::shared_ptr<const IObject> GetSelectedObject() const;
	std::shared_ptr<IObject> GetSelectedObject();
	virtual void SetProperty(std::wstring const& key, std::wstring const& value) override;
	virtual std::wstring GetProperty(std::wstring const& key) const override;
	std::map<std::wstring, std::wstring> const& GetAllProperties() const;
	void AddProjectile(CProjectile const& projectile);
	size_t GetProjectileCount() const;
	CProjectile const& GetProjectile(size_t index) const;
	void AddParticleEffect(const IParticleUpdater * updater, std::wstring const& effectPath, CVector3f const& position, float scale, size_t maxParticles = 1000u);
	size_t GetParticleCount() const;
	CParticleEffect const& GetParticleEffect(size_t index) const;
	void RemoveParticleEffect(size_t index);
	void Update(long long timeSinceLastUpdate);
	void RemoveProjectile(unsigned int index);
	CLandscape & GetLandscape();
	void ResetLandscape(float width, float depth, std::wstring const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth);

	void DoOnObjectCreation(std::function<void(IObject*)> const& handler);
	void DoOnObjectRemove(std::function<void(IObject*)> const& handler);
private:
	CGameModel(CGameModel const&) = delete;

	std::vector<std::shared_ptr<IObject>> m_objects;
	std::vector<CProjectile> m_projectiles;
	std::vector<CParticleEffect> m_particleEffects;
	std::shared_ptr<IObject> m_selectedObject;
	std::map<std::wstring, std::wstring> m_properties;
	CLandscape m_landscape;
	std::function<void(IObject *)> m_onObjectCreation;
	std::function<void(IObject *)> m_onObjectRemove;
};