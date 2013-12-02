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
	CVector3d firstDiagonal[3] = {CVector3d(m_min[0], m_max[1], m_min[2]), CVector3d(m_max[0], m_max[1], m_min[2]), CVector3d(m_min[0], m_min[1], m_max[2])};
	CVector3d secondDiagonal[3] = {CVector3d(m_max[0], m_max[1], m_min[2]), CVector3d(m_max[0], m_min[1], m_min[2]), CVector3d(m_min[0], m_min[1], m_max[2])};

    RotateScaleTranslate(firstDiagonal[0], rotation, m_scale, x, y, z);
	RotateScaleTranslate(firstDiagonal[1], rotation, m_scale, x, y, z);
	RotateScaleTranslate(firstDiagonal[2], rotation, m_scale, x, y, z);
    RotateScaleTranslate(secondDiagonal[0], rotation, m_scale, x, y, z);
	RotateScaleTranslate(secondDiagonal[1], rotation, m_scale, x, y, z);
	RotateScaleTranslate(secondDiagonal[2], rotation, m_scale, x, y, z);

	CVector3d p1(origin[0], origin[1], origin[2]);
	CVector3d p2(end[0], end[1], end[2]);

	CVector3d interesect;
	bool result;
	if (IntersectRayWithSide(firstDiagonal[0], firstDiagonal[1], firstDiagonal[2], p1, p2, intersectCoord))
	{
	   return true;
	}
	if (IntersectRayWithSide(secondDiagonal[0], secondDiagonal[1], secondDiagonal[2], p1, p2, intersectCoord))
	{
	   return true;
	}
	return false;
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

	glColor3d(0.0, 255.0, 255.0);
		//Left
	glBegin(GL_LINE_LOOP);
	glVertex3d(1, 1, 0);
	glVertex3d(1, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);
	glEnd();
	//Right

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