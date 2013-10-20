#include "GameModel.h"
#include "..\SelectionTools.h"
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
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		if(m_objects[i].get() == m_objects[m_SelectedObject].get())
		{
			m_objects.erase(m_objects.begin() + i);
		}
	}
	m_SelectedObject = -1;
}

void CGameModel::SelectObjectByIndex(long index)
{
	m_SelectedObject = index;
}

std::string CGameModel::GetSelectedObjectModel() const
{
	return (m_SelectedObject != -1) ? m_objects[m_SelectedObject]->GetPathToModel() : "";
}

void CGameModel::MoveSelectedObjectTo(double x, double y)
{
	if (m_SelectedObject == -1)
	{
		return;
	}
	m_objects[m_SelectedObject]->MoveTo(x, y, 0);
}

std::shared_ptr<const IObject> CGameModel::GetSelectedObject() const
{
	if (m_SelectedObject == -1)
	{
		return std::shared_ptr<const IObject>(NULL);
	}
	return m_objects[m_SelectedObject];
}