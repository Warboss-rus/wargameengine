#include "Model.h"
#include "../view/ModelManager.h"
#include "../model/ObjectGroup.h"
#include "Object.h"
#include <cstring>

namespace wargameEngine
{
namespace model
{
size_t Model::GetObjectCount() const
{
	return m_objects.size();
}

std::shared_ptr<const IObject> Model::Get3DObject(size_t number) const
{
	return m_objects[number];
}

std::shared_ptr<IObject> Model::Get3DObject(size_t number)
{
	return m_objects[number];
}

std::shared_ptr<IObject> Model::Get3DObject(const IBaseObject * object)
{
	for (auto i = m_objects.begin(); i != m_objects.end(); ++i)
	{
		if (i->get() == object)
		{
			return *i;
		}
	}
	if (object == m_selectedObject.get())
	{
		return m_selectedObject;
	}
	return nullptr;
}

void Model::AddObject(std::shared_ptr<IObject> const& pObject)
{
	m_objects.push_back(pObject);
	m_onObjectCreation(pObject.get());
}

void Model::SelectObject(std::shared_ptr<IObject> const& pObject)
{
	m_selectedObject = pObject;
}

std::shared_ptr<const IObject> Model::GetSelectedObject() const
{
	return m_selectedObject;
}

std::shared_ptr<IObject> Model::GetSelectedObject()
{
	return m_selectedObject;
}

void Model::AddStaticObject(const StaticObject& object)
{
	m_staticObjects.push_back(object);
}

size_t Model::GetStaticObjectCount() const
{
	return m_staticObjects.size();
}

StaticObject& Model::GetStaticObject(size_t index)
{
	return m_staticObjects.at(index);
}

void Model::DeleteObjectByPtr(std::shared_ptr<IObject> const& pObject)
{
	if (pObject.get()->IsGroup())
	{
		ObjectGroup* group = (ObjectGroup*)pObject.get();
		group->DeleteAll();
	}
	if (pObject == m_selectedObject) m_selectedObject = nullptr;
	for (auto i = m_objects.begin(); i != m_objects.end(); ++i)
	{
		if (i->get() == pObject.get())
		{
			m_objects.erase(i);
			break;
		}
	}
	m_onObjectRemove(pObject.get());
}

void Model::Clear()
{
	m_objects.clear();
	m_properties.clear();
}

std::wstring Model::GetProperty(std::wstring const& key) const
{
	if (m_properties.find(key) != m_properties.end())
		return m_properties.find(key)->second;
	return L"";
}

void Model::SetProperty(std::wstring const& key, std::wstring const& value)
{
	m_properties[key] = value;
}

std::map<std::wstring, std::wstring> const&  Model::GetAllProperties() const
{
	return m_properties;
}


void Model::AddProjectile(Projectile const& projectile)
{
	m_projectiles.push_back(projectile);
}

size_t Model::GetProjectileCount() const
{
	return m_projectiles.size();
}

Projectile const& Model::GetProjectile(size_t index) const
{
	return m_projectiles[index];
}

void Model::AddParticleEffect(const IParticleUpdater * updater, const Path& effectPath, CVector3f const& position, float scale, size_t maxParticles)
{
	m_particleEffects.emplace_back(updater, effectPath, position, scale, maxParticles);
}

size_t Model::GetParticleCount() const
{
	return m_particleEffects.size();
}

ParticleEffect const& Model::GetParticleEffect(size_t index) const
{
	return m_particleEffects.at(index);
}

void Model::RemoveParticleEffect(size_t index)
{
	m_particleEffects.erase(m_particleEffects.begin() + index);
}

void Model::ResetLandscape(float width, float depth, const Path& texture, size_t pointsPerWidth, size_t pointsPerDepth)
{
	m_landscape.Reset(width, depth, texture, pointsPerWidth, pointsPerDepth);
}

void Model::AddLight()
{
	CVector3f defaultPos;
	m_lights.emplace_back(defaultPos);
}

void Model::RemoveLight(size_t index)
{
	m_lights.erase(m_lights.begin() + index);
}

Light& Model::GetLight(size_t index)
{
	return m_lights.at(index);
}

const std::vector<Light>& Model::GetLights() const
{
	return m_lights;
}

signals::SignalConnection Model::DoOnObjectCreation(std::function<void(IObject*)> const& handler)
{
	return m_onObjectCreation.Connect(handler);
}

signals::SignalConnection Model::DoOnObjectRemove(std::function<void(IObject*)> const& handler)
{
	return m_onObjectRemove.Connect(handler);
}

std::unique_lock<std::mutex> Model::LockModel()
{
	return std::unique_lock<std::mutex>(m_modelLock);
}

void Model::Update(std::chrono::microseconds timeSinceLastUpdate)
{
	for (size_t i = 0; i < m_objects.size(); ++i)
	{
		m_objects[i]->Update(timeSinceLastUpdate);
	}
	for (size_t i = 0; i < m_projectiles.size(); ++i)
	{
		m_projectiles[i].Update(timeSinceLastUpdate);
	}
	for (auto& effect : m_particleEffects)
	{
		effect.Update(timeSinceLastUpdate);
	}
}

void Model::RemoveProjectile(unsigned int index)
{
	m_projectiles.erase(m_projectiles.begin() + index);
}

Landscape & Model::GetLandscape()
{
	return m_landscape;
}
}
}