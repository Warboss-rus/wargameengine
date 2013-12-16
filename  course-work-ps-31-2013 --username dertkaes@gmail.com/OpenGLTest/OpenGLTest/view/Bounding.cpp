#include "Bounding.h"
#include <cstring>
#include <GL\glut.h>
#include <fstream>
#include <string>
#include "Vector3.h"
#include <algorithm>

const double PI = 3.14159265359;

CBoundingBox::CBoundingBox(double min[3], double max[3]) 
{ 
	memcpy(m_min, min, sizeof(double) * 3); 
	memcpy(m_max, max, sizeof(double) * 3); 
}

void RotateScaleTranslate(double& x, double& y, double& z, double angle, double scale, double transX, double transY, double transZ)
{
	angle *= (PI / 180);
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

void RotateScaleTranslate(CVector3d& vector, double angle, double scale, double transX, double transY, double transZ)
{
	angle *= (PI / 180);
	double newx = vector.x * cos(angle) - vector.y * sin(angle);//rotate
	vector.y = vector.x * sin(angle) + vector.y * cos(angle);
	vector.x = newx;
	vector.x *= scale;//scale
	vector.y *= scale;
	vector.z *= scale;
	vector.x += transX;//translate
	vector.y += transY;
	vector.z += transZ;
}

bool PointInSegment(CVector3d const& p, CVector3d const& pa, CVector3d const& pb, CVector3d const& pc)
{
	double E = 1e-5;
	if (p.x - E > std::max(pa.x, std::max(pb.x, pc.x)) || p.x + E < std::min(pa.x, std::min(pb.x, pc.x)))
	{
		return false;
	}
	if (p.y - E > std::max(pa.y, std::max(pb.y, pc.y)) || p.y + E < std::min(pa.y, std::min(pb.y, pc.y)))
	{
		return false;
	}
	if (p.z - E > std::max(pa.z, std::max(pb.z, pc.z)) || p.z + E < std::min(pa.z, std::min(pb.z, pc.z)))
	{
		return false;
	}
	return true;
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

bool IntersectRayWithSide(CVector3d const& pa, CVector3d const& pb, CVector3d const& pc, CVector3d const& startPointRay, CVector3d const& endPointRay, CVector3d& interesect)
{
   double d, denom, mu;
   CVector3d n;

   /* Calculate the parameters for the plane */
   n.x = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
   n.y = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
   n.z = (pb.x - pa.x)*(pc.y - pa.y) - (pb.y - pa.y)*(pc.x - pa.x);
   n.Normalize();
   d = - n.x * pa.x - n.y * pa.y - n.z * pa.z;

   double E = 1e-5;
   /* Calculate the position on the line that intersects the plane */
   denom = n.x * (endPointRay.x - startPointRay.x) + n.y * (endPointRay.y - startPointRay.y) + n.z * (endPointRay.z - startPointRay.z);
   if (abs(denom) < E)         /* Line and plane don't intersect */
      return false;
   mu = - (d + n.x * startPointRay.x + n.y * startPointRay.y + n.z * startPointRay.z) / denom;
   interesect.x = startPointRay.x + mu * (endPointRay.x - startPointRay.x);
   interesect.y = startPointRay.y + mu * (endPointRay.y - startPointRay.y);
   interesect.z = startPointRay.z + mu * (endPointRay.z - startPointRay.z);
   return PointInSegment(interesect, pa, pb, pc);
}

bool CBoundingBox::IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const
{
/*	CVector3d side1[3] = {CVector3d(m_min[0], m_max[1], m_min[2]), CVector3d(m_max[0], m_max[1], m_min[2]), CVector3d(m_min[0], m_min[1], m_min[2])};
	CVector3d side2[3] = {CVector3d(m_min[0], m_max[1], m_min[2]), CVector3d(m_max[0], m_max[1], m_min[2]), CVector3d(m_min[0], m_max[1], m_max[2])};
	CVector3d side3[3] = {CVector3d(m_min[0], m_max[1], m_max[2]), CVector3d(m_max[0], m_max[1], m_max[2]), CVector3d(m_max[0], m_min[1], m_max[2])};
	CVector3d side4[3] = {CVector3d(m_min[0], m_max[1], m_max[2]), CVector3d(m_min[0], m_min[1], m_max[2]), CVector3d(m_min[0], m_min[1], m_min[2])};
	CVector3d side5[3] = {CVector3d(m_min[0], m_min[1], m_min[2]), CVector3d(m_max[0], m_min[1], m_min[2]), CVector3d(m_max[0], m_min[1], m_max[2])};
	CVector3d side6[3] = {CVector3d(m_max[0], m_max[1], m_max[2]), CVector3d(m_max[0], m_min[1], m_max[2]), CVector3d(m_max[0], m_min[1], m_min[2])};

	RotateScaleTranslate(side1[0], rotation, m_scale, x, y, z); RotateScaleTranslate(side1[1], rotation, m_scale, x, y, z); RotateScaleTranslate(side1[2], rotation, m_scale, x, y, z); 
	RotateScaleTranslate(side2[0], rotation, m_scale, x, y, z); RotateScaleTranslate(side2[1], rotation, m_scale, x, y, z); RotateScaleTranslate(side2[2], rotation, m_scale, x, y, z); 
	RotateScaleTranslate(side3[0], rotation, m_scale, x, y, z); RotateScaleTranslate(side3[1], rotation, m_scale, x, y, z); RotateScaleTranslate(side3[2], rotation, m_scale, x, y, z); 
	RotateScaleTranslate(side4[0], rotation, m_scale, x, y, z); RotateScaleTranslate(side4[1], rotation, m_scale, x, y, z); RotateScaleTranslate(side4[2], rotation, m_scale, x, y, z); 
	RotateScaleTranslate(side5[0], rotation, m_scale, x, y, z); RotateScaleTranslate(side5[1], rotation, m_scale, x, y, z); RotateScaleTranslate(side5[2], rotation, m_scale, x, y, z); 
	RotateScaleTranslate(side6[0], rotation, m_scale, x, y, z); RotateScaleTranslate(side6[1], rotation, m_scale, x, y, z); RotateScaleTranslate(side6[2], rotation, m_scale, x, y, z); 


	CVector3d p1(origin[0], origin[1], origin[2]);
	CVector3d p2(end[0], end[1], end[2]);
	if (IntersectRayWithSide(side1[0], side1[1], side1[2], p1, p2, intersectCoord))
	{
	   return true;
	}
	if (IntersectRayWithSide(side2[0], side2[1], side2[2], p1, p2, intersectCoord))
	{
	   return true;
	}
	if (IntersectRayWithSide(side3[0], side3[1], side3[2], p1, p2, intersectCoord))
	{
	   return true;
	}
	if (IntersectRayWithSide(side4[0], side4[1], side4[2], p1, p2, intersectCoord))
	{
	   return true;
	}
	if (IntersectRayWithSide(side5[0], side5[1], side5[2], p1, p2, intersectCoord))
	{
	   return true;
	}
	if (IntersectRayWithSide(side6[0], side6[1], side6[2], p1, p2, intersectCoord))
	{
	   return true;
	}
	return false;*/
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
	double dir[3] = {end[0] - origin[0], end[1] - origin[1], end[2] - origin[2]};

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

bool CBoundingCompound::IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const
{
	for(size_t i = 0; i < m_children.size(); ++i)
	{
		if(m_children[i]->IsIntersectsRay(origin, end, x, y, z, rotation, intersectCoord))
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