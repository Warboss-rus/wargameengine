#include "GameModel.h"
#include "..\view\ModelManager.h"

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

void CGameModel::AddObject(std::shared_ptr<IObject> pObject)
{
	m_objects.push_back(pObject);
}

void CGameModel::DeleteSelectedObject()
{
	if(m_selectedObjectIndex == NULL_INDEX) return;
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		if(m_objects[i].get() == m_objects[m_selectedObjectIndex].get())
		{
			m_objects.erase(m_objects.begin() + i);
		}
	}
	m_selectedObjectIndex = NULL_INDEX;
}

void CGameModel::SelectObjectByIndex(long index)
{
	m_selectedObjectIndex = index;
}

std::string CGameModel::GetSelectedObjectModel() const
{
	return (m_selectedObjectIndex != NULL_INDEX) ? m_objects[m_selectedObjectIndex]->GetPathToModel() : "";
}

void CGameModel::MoveSelectedObjectTo(double x, double y)
{
	if (m_selectedObjectIndex == NULL_INDEX)
	{
		return;
	}
	m_objects[m_selectedObjectIndex]->MoveTo(x, y, 0);
}

std::shared_ptr<const IObject> CGameModel::GetSelectedObject() const
{
	if (m_selectedObjectIndex == NULL_INDEX)
	{
		return std::shared_ptr<const IObject>(NULL);
	}
	return m_objects[m_selectedObjectIndex];
}

void CGameModel::DeleteObjectByPtr(std::shared_ptr<IObject> pObject)
{
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		if(m_objects[i].get() == pObject.get())
		{
			m_objects.erase(m_objects.begin() + i);
		}
	}
	if(m_selectedObjectIndex != NULL_INDEX && pObject == m_objects[m_selectedObjectIndex]) m_selectedObjectIndex = NULL_INDEX;
}