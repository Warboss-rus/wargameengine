#include "Bounding.h"
#include <cstring>
#include <GL\glut.h>
#include <fstream>
#include <string>

CBoundingBox::CBoundingBox(double min[3], double max[3]) 
{ 
	memcpy(m_min, min, sizeof(double) * 3); 
	memcpy(m_max, max, sizeof(double) * 3); 
}

void RotateScaleTranslate(double & x, double & y, double & z, double angle, double scale, double transX, double transY, double transZ)
{
	angle *= 3.1417 / 180;
	double newx = x * cos(angle) - y * sin(angle);//rotate
	y = x * sin(angle) + y * cos(angle);
	x = newx;
	x *= scale;//scale
	y *= scale;
	z *= scale;
	x += transX;//translate
	y += transY;
	z += transZ;
}

void RotateScaleTranslate2(double & x, double & y, double & z, double angle, double scale, double transX, double transY, double transZ)
{
	x *= scale;//scale
	y *= scale;
	z *= scale;
	x += transX;//translate
	y += transY;
	z += transZ;
}

bool CBoundingBox::IsIntersectsRay(double origin[3], double dir[3], double x, double y, double z, double rotation, sPoint3 & intersectCoord) const
{
	double minB[3] = {m_min[0], m_min[1], m_min[2]};
	double maxB[3] = {m_max[0], m_max[1], m_max[2]};
	RotateScaleTranslate2(minB[0], minB[1], minB[2], rotation, m_scale, x, y, z);
	RotateScaleTranslate2(maxB[0], maxB[1], maxB[2], rotation, m_scale, x, y, z);
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
	intersectCoord.x = coord[0];
	intersectCoord.y = coord[1];
	intersectCoord.z = coord[2];
	return true;				/* ray hits box */
}

void CBoundingBox::Draw(double x, double y, double z, double rotation) const
{
	double box[24] = {m_min[0], m_min[1], m_min[2],
		m_min[0], m_max[1], m_min[2], 
		m_max[0], m_max[1], m_min[2], 
		m_max[0], m_min[1], m_min[2],
		m_min[0], m_min[1], m_max[2], 
		m_min[0], m_max[1], m_max[2], 
		m_max[0], m_max[1], m_max[2], 
		m_max[0], m_min[1], m_max[2]};
	RotateScaleTranslate(box[0], box[1], box[2], rotation, m_scale, x, y, z);
	RotateScaleTranslate(box[3], box[4], box[5], rotation, m_scale, x, y, z);
	RotateScaleTranslate(box[6], box[7], box[8], rotation, m_scale, x, y, z);
	RotateScaleTranslate(box[9], box[10], box[11], rotation, m_scale, x, y, z);
	RotateScaleTranslate(box[12], box[13], box[14], rotation, m_scale, x, y, z);
	RotateScaleTranslate(box[15], box[16], box[17], rotation, m_scale, x, y, z);
	RotateScaleTranslate(box[18], box[19], box[20], rotation, m_scale, x, y, z);
	RotateScaleTranslate(box[21], box[22], box[23], rotation, m_scale, x, y, z);
	glPushMatrix();
	glColor3d(0.0, 0.0, 255.0);
	//Left
	glBegin(GL_LINE_LOOP);
	glVertex3d(box[9], box[10], box[11]);
	glVertex3d(box[6], box[7], box[8]);
	glVertex3d(box[18], box[19], box[20]);
	glVertex3d(box[21], box[22], box[23]);
	glEnd();
	//Right
	glBegin(GL_LINE_LOOP);
	glVertex3d(box[0], box[1], box[2]);
	glVertex3d(box[3], box[4], box[5]);
	glVertex3d(box[15], box[16], box[17]);
	glVertex3d(box[12], box[13], box[14]);
	glEnd();
	//Front
	glBegin(GL_LINE_LOOP);
	glVertex3d(box[0], box[1], box[2]);
	glVertex3d(box[9], box[10], box[11]);
	glVertex3d(box[21], box[22], box[23]);
	glVertex3d(box[12], box[13], box[14]);
	glEnd();
	//Back
	glBegin(GL_LINE_LOOP);
	glVertex3d(box[3], box[4], box[5]);
	glVertex3d(box[6], box[7], box[8]);
	glVertex3d(box[18], box[19], box[20]);
	glVertex3d(box[15], box[16], box[17]);
	glEnd();
	glColor3d(255.0, 255.0, 255.0);
	glPopMatrix();
}

void CBoundingCompound::AddChild(std::shared_ptr<IBounding> child)
{
	m_children.push_back(child);
}

void CBoundingCompound::Draw(double x, double y, double z, double rotation) const
{
	for(size_t i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->Draw(x, y, z, rotation);
	}
}

bool CBoundingCompound::IsIntersectsRay(double origin[3], double dir[3], double x, double y, double z, double rotation, sPoint3 & intersectCoord) const
{
	for(size_t i = 0; i < m_children.size(); ++i)
	{
		if(m_children[i]->IsIntersectsRay(origin, dir, x, y, z, rotation, intersectCoord))
		{
			return true;
		}
	}
	return false;
}
void CBoundingCompound::SetScale(double scale)
{
	for(size_t i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->SetScale(scale);
	}
}

std::shared_ptr<IBounding> LoadBoundingFromFile(std::string const& path, double & scale)
{
	std::ifstream iFile(path);
	std::shared_ptr<IBounding> bounding (new CBoundingCompound());
	std::string line;
	unsigned int count = 0;
	while(iFile.good())
	{
		iFile >> line;
		if(line == "box")
		{
			double min[3], max[3];
			iFile >> min[0] >> min[1] >> min[2] >> max[0] >> max[1] >> max[2];
			std::shared_ptr<IBounding> current(new CBoundingBox(min, max));
			CBoundingCompound * compound = (CBoundingCompound *)bounding.get();
			compound->AddChild(current);
		}
		if(line == "scale")
		{
			iFile >> scale;
		}
	}
	iFile.close();
	CBoundingCompound * compound = (CBoundingCompound *)bounding.get();
	compound->SetScale(scale);
	return bounding;
}