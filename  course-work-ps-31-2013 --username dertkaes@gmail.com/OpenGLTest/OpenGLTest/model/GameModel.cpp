#include "GameModel.h"
#include "..\view\ModelManager.h"
#include "..\ObjectGroup.h"

std::shared_ptr<CGameModel> CGameModel::m_instanse = NULL;

CGameModel::~CGameModel(void)
{
}

std::weak_ptr<CGameModel> CGameModel::GetIntanse()
{
	if (!m_instanse)
	{
		m_instanse.reset(new CGameModel());
	}
	return std::weak_ptr<CGameModel>(m_instanse);
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
	if(IsGroup(pObject))
	{
		CObjectGroup* group = (CObjectGroup*)pObject.get();
		group->DeleteAll();
	}
	if(pObject == m_selectedObject) m_selectedObject = NULL;
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		if(m_objects[i].get() == pObject.get())
		{
			
			m_objects.erase(m_objects.begin() + i);
		}
	}
}

bool CGameModel::IsGroup(std::shared_ptr<IObject> object)
{
	return dynamic_cast<CObjectGroup *>(object.get()) != NULL;
}