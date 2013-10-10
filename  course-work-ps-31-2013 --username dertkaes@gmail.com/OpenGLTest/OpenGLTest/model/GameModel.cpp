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
		if(m_objects[i].get() == m_pSelectedObject)
		{
			m_objects.erase(m_objects.begin() + i);
		}
	}
	m_pSelectedObject = NULL;
}

bool CGameModel::TrySelectObject(double begin[3], double end[3])
{
	return m_pSelectedObject = Get3DObjectIntersectingRay(begin, end);
}

IObject* CGameModel::Get3DObjectIntersectingRay(double begin[3], double end[3])
{
	IObject* selectedObject = NULL;
	double minDistance = 10000000.0;
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		const double *bounding = m_objects[i]->GetBounding();
		double minBox[3] = {bounding[0], bounding[1], bounding[2]};
		double maxBox[3] = {bounding[3], bounding[4], bounding[5]};
		double direction[3] = {end[0] - begin[0], end[1] - begin[1], end[2] - begin[2]};
		if(BoxRayIntersect(minBox, maxBox, begin, direction))
		{
			double distance = sqrt(m_objects[i]->GetX() * m_objects[i]->GetX() + m_objects[i]->GetY() * m_objects[i]->GetY() + 
				m_objects[i]->GetZ() * m_objects[i]->GetZ());
			if(distance < minDistance)
			{
				selectedObject = m_objects[i].get();
				minDistance = distance;
			}
		}
	}
	return selectedObject;
}

const double * CGameModel::GetSelectedObjectBoundingBox() const
{
	return (m_pSelectedObject) ? m_pSelectedObject->GetBounding() : NULL;
}

void CGameModel::MoveSelectedObjectTo(double x, double y)
{
	if (!m_pSelectedObject)
	{
		return;
	}
	m_pSelectedObject->MoveTo(x, y, 0);
}