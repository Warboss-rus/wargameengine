#include "GameModel.h"
#include "../view/ModelManager.h"
#include "../model/ObjectGroup.h"
#include "Object.h"
#include <cstring>

size_t CGameModel::GetObjectCount() const
{
	return m_objects.size();
}

std::shared_ptr<const IObject> CGameModel::Get3DObject(size_t number) const
{
	return m_objects[number];
}

std::shared_ptr<IObject> CGameModel::Get3DObject(size_t number)
{
	return m_objects[number];
}

std::shared_ptr<IObject> CGameModel::Get3DObject(const IBaseObject * object)
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

void CGameModel::AddObject(std::shared_ptr<IObject> const& pObject)
{
	m_objects.push_back(pObject);
	m_onObjectCreation(pObject.get());
}

void CGameModel::SelectObject(std::shared_ptr<IObject> const& pObject)
{
	m_selectedObject = pObject;
}

std::shared_ptr<const IObject> CGameModel::GetSelectedObject() const
{
	return m_selectedObject;
}

std::shared_ptr<IObject> CGameModel::GetSelectedObject()
{
	return m_selectedObject;
}

void CGameModel::DeleteObjectByPtr(std::shared_ptr<IObject> const& pObject)
{
	if(pObject.get()->IsGroup())
	{
		CObjectGroup* group = (CObjectGroup*)pObject.get();
		group->DeleteAll();
	}
	if(pObject == m_selectedObject) m_selectedObject = nullptr;
	for(auto i = m_objects.begin(); i != m_objects.end(); ++i)
	{
		if(i->get() == pObject.get())
		{
			m_objects.erase(i);
			return;
		}
	}
	m_onObjectRemove(pObject.get());
}

void CGameModel::Clear()
{
	m_objects.clear();
	m_properties.clear();
}

std::wstring CGameModel::GetProperty(std::wstring const& key) const
{
	if (m_properties.find(key) != m_properties.end())
		return m_properties.find(key)->second;
	return L"";
}

void CGameModel::SetProperty(std::wstring const& key, std::wstring const& value)
{ 
	m_properties[key] = value; 
}

std::map<std::wstring, std::wstring> const&  CGameModel::GetAllProperties() const
{ 
	return m_properties; 
}


void CGameModel::AddProjectile(CProjectile const& projectile)
{
	m_projectiles.push_back(projectile);
}

size_t CGameModel::GetProjectileCount() const
{
	return m_projectiles.size();
}

CProjectile const& CGameModel::GetProjectile(size_t index) const
{
	return m_projectiles[index];
}

void CGameModel::AddParticleEffect(const IParticleUpdater * updater, std::wstring const& effectPath, CVector3f const& position, float scale, size_t maxParticles)
{
	m_particleEffects.emplace_back(updater, effectPath, position, scale, maxParticles);
}

size_t CGameModel::GetParticleCount() const
{
	return m_particleEffects.size();
}

CParticleEffect const& CGameModel::GetParticleEffect(size_t index) const
{
	return m_particleEffects.at(index);
}

void CGameModel::RemoveParticleEffect(size_t index)
{
	m_particleEffects.erase(m_particleEffects.begin() + index);
}

void CGameModel::ResetLandscape(float width, float depth, std::wstring const& texture, size_t pointsPerWidth, size_t pointsPerDepth)
{
	m_landscape.Reset(width, depth, texture, pointsPerWidth, pointsPerDepth);
}

void CGameModel::AddLight()
{
	CVector3f defaultPos;
	m_lights.emplace_back(defaultPos);
}

void CGameModel::RemoveLight(size_t index)
{
	m_lights.erase(m_lights.begin() + index);
}

CLight& CGameModel::GetLight(size_t index)
{
	return m_lights.at(index);
}

const std::vector<CLight>& CGameModel::GetLights() const
{
	return m_lights;
}

CSignalConnection<void, IObject*> CGameModel::DoOnObjectCreation(std::function<void(IObject*)> const& handler)
{
	return m_onObjectCreation.Connect(handler);
}

CSignalConnection<void, IObject*> CGameModel::DoOnObjectRemove(std::function<void(IObject*)> const& handler)
{
	return m_onObjectRemove.Connect(handler);
}

std::unique_lock<std::mutex> CGameModel::LockModel()
{
	return std::unique_lock<std::mutex>(m_modelLock);
}

void CGameModel::Update(long long timeSinceLastUpdate)
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

void CGameModel::RemoveProjectile(unsigned int index)
{
	m_projectiles.erase(m_projectiles.begin() + index);
}

CLandscape & CGameModel::GetLandscape()
{
	return m_landscape;
}
