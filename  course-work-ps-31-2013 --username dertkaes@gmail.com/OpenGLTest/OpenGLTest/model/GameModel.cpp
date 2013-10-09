#include "GameModel.h"

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

IObject* CGameModel::Get3DObject(unsigned long number) const
{
	return m_objects[number];
}

void CGameModel::AddObject(IObject * object)
{
	m_objects.push_back(object);
}

void CGameModel::DeleteObject(IObject * object)
{
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		if(m_objects[i] == object)
		{
			delete m_objects[i];
			m_objects.erase(m_objects.begin() + i);
		}
	}
}
