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

std::shared_ptr<IObject> CGameModel::Get3DObject(IObject * object)
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

void CGameModel::AddObject(std::shared_ptr<IObject> pObject)
{
	m_objects.push_back(pObject);
}

void CGameModel::SelectObject(std::shared_ptr<IObject> pObject)
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

void CGameModel::DeleteObjectByPtr(std::shared_ptr<IObject> pObject)
{
	if(IsGroup(pObject.get()))
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
}

bool CGameModel::IsGroup(IObject* object)
{
	return dynamic_cast<CObjectGroup *>(object) != nullptr;
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

void CGameModel::ResetLandscape(double width, double depth, std::wstring const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth)
{
	m_landscape.Reset(width, depth, texture, pointsPerWidth, pointsPerDepth);
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
}

void CGameModel::RemoveProjectile(unsigned int index)
{
	m_projectiles.erase(m_projectiles.begin() + index);
}

CLandscape & CGameModel::GetLandscape()
{
	return m_landscape;
}
