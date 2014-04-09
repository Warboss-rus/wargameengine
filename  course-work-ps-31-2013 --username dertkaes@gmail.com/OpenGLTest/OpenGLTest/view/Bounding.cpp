#include "Bounding.h"
#include <cstring>
#include <GL\glut.h>
#include <fstream>
#include <string>
#include "Vector3.h"
#include <algorithm>
#include "Geometry/Ray.h"
#include "Geometry/Polyhedron.h"
#include "Geometry/LineSegment.h"
#include "Geometry/Line.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "Math/float3.h"
#include "Math/float3x3.h"

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

AABB GetAABB(CBoundingBox const& bounding)
{
	const double *min = bounding.GetMin();
	const double *max = bounding.GetMax();
	float min1[] = {min[0], min[1], min[2]};
	float max1[] = {max[0], max[1], max[2]};
	math::float3 minVector(min1);
	math::float3 maxVector(max1);
	return AABB(minVector, maxVector);
}

OBB GetOBB(const CBoundingBox const& bounding, float3 translate, float angle)
{
	OBB res( GetAABB(bounding) );
	float z = res.r.z;
	res.Scale( res.CenterPoint(), bounding.GetScale() );
	res.Translate(float3(0, 0, res.r.z - z));

	float3x3 rotateMatrix(
		cos(angle), -sin(angle), 0,
		sin(angle), cos(angle), 0,
		0, 0, 1
	);
	res.Transform(rotateMatrix);
	res.Translate(translate);
	return res;
}

bool CBoundingBox::IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const
{
	float origin1[] = {origin[0], origin[1], origin[2]};
	float3 pos(origin1);
	float3 dir(end[0] - origin[0], end[1] - origin[1], end[2] - origin[2]);
	Line l(pos, dir);

	OBB object = GetOBB(*this, float3(x, y, z), rotation);
	
	if ( object.Intersects(l) )
	{
		Polyhedron polyhedron = object.ToPolyhedron();
		float end1[] = {end[0], end[1], end[2]};
		float3 endPont(end1);
		LineSegment line(pos, endPont);
		float3 interesectPoint = polyhedron.ClosestPoint(line);
		intersectCoord.x = interesectPoint.x;
		intersectCoord.y = interesectPoint.y;
		intersectCoord.z = interesectPoint.z;
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