#include "GameModel.h"
#include "../view/ModelManager.h"
#include "../model/ObjectGroup.h"
#include "Object.h"
#include <cstring>

std::shared_ptr<CGameModel> CGameModel::m_instanse = NULL;

std::weak_ptr<CGameModel> CGameModel::GetInstance()
{
	if (!m_instanse)
	{
		m_instanse.reset(new CGameModel());
	}
	return std::weak_ptr<CGameModel>(m_instanse);
}

void CGameModel::FreeInstance() 
{
	m_instanse.reset();
}

unsigned long CGameModel::GetObjectCount() const
{
	return m_objects.size();
}

std::shared_ptr<const IObject> CGameModel::Get3DObject(unsigned long number) const
{
	return m_objects[number];
}

std::shared_ptr<IObject> CGameModel::Get3DObject(unsigned long number)
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
	return dynamic_cast<CObjectGroup *>(object) != NULL;
}

void CGameModel::Clear()
{
	m_objects.clear();
	m_properties.clear();
}

std::string const CGameModel::GetProperty(std::string const& key) 
{
	if (m_properties.find(key) != m_properties.end())
		return m_properties.find(key)->second;
	return "";
}

void CGameModel::SetProperty(std::string const& key, std::string const& value)
{ 
	m_properties[key] = value; 
}

std::map<std::string, std::string> const&  CGameModel::GetAllProperties() const
{ 
	return m_properties; 
}


void CGameModel::ResetLandscape(double width, double depth, std::string const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth)
{
	m_landscape = CLandscape(width, depth, texture, pointsPerWidth, pointsPerDepth);
}

void CGameModel::Update()
{
	for (unsigned int i = 0; i < m_objects.size(); ++i)
	{
		m_objects[i]->Update();
	}
	for (unsigned int i = 0; i < m_projectiles.size(); ++i)
	{
		m_projectiles[i].Update();
	}
}