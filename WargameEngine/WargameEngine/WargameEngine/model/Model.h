#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "../Signal.h"
#include "IModel.h"
#include "IObject.h"
#include "Projectile.h"
#include "ParticleEffect.h"
#include "Landscape.h"
#include "Light.h"
#include <mutex>

namespace wargameEngine
{
namespace model
{
class Model : public IModel
{
public:
	Model() = default;

	virtual size_t GetObjectCount() const override;
	void Clear();
	std::shared_ptr<const IObject> Get3DObject(size_t number) const;
	virtual std::shared_ptr<IObject> Get3DObject(size_t number) override;
	virtual std::shared_ptr<IObject> Get3DObject(const IBaseObject * obj) override;
	virtual void AddObject(std::shared_ptr<IObject> const&) override;
	virtual void DeleteObjectByPtr(std::shared_ptr<IObject> const& pObject) override;
	void SelectObject(std::shared_ptr<IObject> const& pObject);
	std::shared_ptr<const IObject> GetSelectedObject() const;
	std::shared_ptr<IObject> GetSelectedObject();
	void AddStaticObject(const StaticObject& object);
	size_t GetStaticObjectCount() const;
	StaticObject& GetStaticObject(size_t index);
	virtual void SetProperty(std::wstring const& key, std::wstring const& value) override;
	virtual std::wstring GetProperty(std::wstring const& key) const override;
	std::unordered_map<std::wstring, std::wstring> const& GetAllProperties() const;
	void AddProjectile(Projectile const& projectile);
	size_t GetProjectileCount() const;
	Projectile& GetProjectile(size_t index);
	void AddParticleEffect(const IParticleUpdater * updater, const Path& effectPath, CVector3f const& position, float scale, size_t maxParticles = 1000u);
	size_t GetParticleCount() const;
	ParticleEffect const& GetParticleEffect(size_t index) const;
	void RemoveParticleEffect(size_t index);
	void Update(std::chrono::microseconds timeSinceLastUpdate);
	void RemoveProjectile(unsigned int index);
	Landscape & GetLandscape();
	void ResetLandscape(float width, float depth, const Path& texture, size_t pointsPerWidth, size_t pointsPerDepth);
	void AddLight();
	void RemoveLight(size_t index);
	Light& GetLight(size_t index);
	const std::vector<Light>& GetLights() const;
	std::vector<IBaseObject*> GetAllBaseObjects();

	signals::SignalConnection DoOnObjectCreation(std::function<void(IObject*)> const& handler);
	signals::SignalConnection DoOnObjectRemove(std::function<void(IObject*)> const& handler);
	std::unique_lock<std::mutex> LockModel();
private:
	Model(const Model&) = delete;
	Model(Model&&) = delete;
	Model& operator=(const Model&) = delete;
	Model& operator=(Model&&) = delete;

	std::vector<std::shared_ptr<IObject>> m_objects;
	std::vector<StaticObject> m_staticObjects;
	std::vector<Projectile> m_projectiles;
	std::vector<ParticleEffect> m_particleEffects;
	std::shared_ptr<IObject> m_selectedObject;
	std::unordered_map<std::wstring, std::wstring> m_properties;
	Landscape m_landscape;
	std::vector<Light> m_lights;
	signals::Signal<void, IObject *> m_onObjectCreation;
	signals::Signal<void, IObject *> m_onObjectRemove;
	std::mutex m_modelLock;
};
}
}