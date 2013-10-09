#include "GameModel.h"
#include "..\SelectionTools.h"

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

IObject * CGameModel::Get3DObjectIntersectingRay(double begin[3], double end[3]) const
{
	IObject* selectedObject;
	double minDistance = 10000000.0;
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		const double *bounding = m_objects[i]->GetBounding();
		double minBox[3] = {bounding[0] + m_objects[i]->GetX(), bounding[1] + m_objects[i]->GetY(), bounding[2] + m_objects[i]->GetZ()};
		double maxBox[3] = {bounding[3] + m_objects[i]->GetX(), bounding[4] + m_objects[i]->GetY(), bounding[5] + m_objects[i]->GetZ()};
		double direction[3] = {end[0] - begin[0], end[1] - begin[1], end[2] - begin[2]};
		if(BoxRayIntersect(minBox, maxBox, begin, direction))
		{
			double distance = sqrt(m_objects[i]->GetX() * m_objects[i]->GetX() + m_objects[i]->GetY() * m_objects[i]->GetY() + 
				m_objects[i]->GetZ() * m_objects[i]->GetZ());
			if(distance < minDistance)
			{
				selectedObject = m_objects[i];
				minDistance = distance;
			}
		}
	}
	return selectedObject;
}