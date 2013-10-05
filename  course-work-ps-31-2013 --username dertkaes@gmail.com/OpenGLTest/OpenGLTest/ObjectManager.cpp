#include "ObjectManager.h"

void CObjectManager::Draw()
{
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		m_objects[i]->Draw();
	}
}

void CObjectManager::AddObject(IObject * object)
{
	m_objects.push_back(object);
}

void CObjectManager::DeleteObject(IObject * object)
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

bool BoxRayIntersect(double minB[3], double maxB[3], double origin[3], double dir[3])
{
	double coord[3];
	char inside = true;
	char quadrant[3];
	register int i;
	int whichPlane;
	double maxT[3];
	double candidatePlane[3];

	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for (i=0; i<3; i++)
		if(origin[i] < minB[i]) {
			quadrant[i] = 1;
			candidatePlane[i] = minB[i];
			inside = false;
		}else if (origin[i] > maxB[i]) {
			quadrant[i] = 0;
			candidatePlane[i] = maxB[i];
			inside = false;
		}else	{
			quadrant[i] = 2;
		}

	/* Ray origin inside bounding box */
	if(inside)	{
		coord[0] = origin[0];
		coord[1] = origin[1];
		coord[2] = origin[2];
		return true;
	}

	/* Calculate T distances to candidate planes */
	for (i = 0; i < 3; i++)
		if (quadrant[i] != 2 && dir[i] !=0.)
			maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (i = 1; i < 3; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.) return (false);
	for (i = 0; i < 3; i++)
		if (whichPlane != i) {
			coord[i] = origin[i] + maxT[whichPlane] *dir[i];
			if (coord[i] < minB[i] || coord[i] > maxB[i])
				return (false);
		} else {
			coord[i] = candidatePlane[i];
		}
	return true;				/* ray hits box */
}



IObject * CObjectManager::GetNearestObjectByVector(double startx, double starty, double startz, double endx, double endy, double endz)
{
	IObject* selectedObject = NULL;
	double minDistance = 10000000.0;
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		const double *bounding = m_objects[i]->GetBounding();
		double minBox[3] = {bounding[0] + m_objects[i]->GetX(), bounding[1] + m_objects[i]->GetY(), bounding[2] + m_objects[i]->GetZ()};
		double maxBox[3] = {bounding[3] + m_objects[i]->GetX(), bounding[4] + m_objects[i]->GetY(), bounding[5] + m_objects[i]->GetZ()};
		double start[3] = {startx, starty, startz};
		double direction[3] = {endx - startx, endy - starty, endz - startz};
		if(BoxRayIntersect(minBox, maxBox, start, direction))
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