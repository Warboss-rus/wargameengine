#include "Bounding.h"
#include <cstring>
#include "../view/gl.h"
#include <fstream>
#include <string>
#include "Vector3.h"
#include <algorithm>
#include "Matrix3.h"
#include "../lib/src/Geometry/Ray.h"
#include "../lib/src/Geometry/Polyhedron.h"
#include "../lib/src/Geometry/LineSegment.h"
#include "../lib/src/Geometry/Line.h"

const float PI = 3.14159265359;

CBoundingBox::CBoundingBox(float min[3], float max[3])
{ 
	memcpy(m_min, min, sizeof(float) * 3); 
	memcpy(m_max, max, sizeof(float) * 3); 
}

void RotateScaleTranslate(float& x, float& y, float& z, float angle, float scale, float transX, float transY, float transZ)
{
	angle *= (PI / 180);
	float newx = x * cos(angle) - y * sin(angle);//rotate
	y = x * sin(angle) + y * cos(angle);
	x = newx;
	x *= scale;//scale
	y *= scale;
	z *= scale;
	x += transX;//translate
	y += transY;
	z += transZ;
}

void RotateScaleTranslate(CVector3d& vector, float angle, float scale, float transX, float transY, float transZ)
{
	angle *= (PI / 180);
	float newx = vector.x * cos(angle) - vector.y * sin(angle);//rotate
	vector.y = vector.x * sin(angle) + vector.y * cos(angle);
	vector.x = newx;
	vector.x *= scale;//scale
	vector.y *= scale;
	vector.z *= scale;
	vector.x += transX;//translate
	vector.y += transY;
	vector.z += transZ;
}

void RotateScaleTranslate2(float & x, float & y, float & z, float angle, float scale, float transX, float transY, float transZ)
{
	x *= scale;//scale
	y *= scale;
	z *= scale;
	x += transX;//translate
	y += transY;
	z += transZ;
}

bool CBoundingBox::IsIntersectsRay(float origin[3], float end[3], float x, float y, float z, float rotation, CVector3d & intersectCoord) const
{
	CBoundingBox box(*this);
	float3 pos(origin);
	float3 dir(end[0] - origin[0], end[1] - origin[1], end[2] - origin[2]);
	Line l(pos, dir);

	OBB object = GetOBB(*this, float3(x, y, z), rotation);
	
	if ( object.Intersects(l) )
	{
		Polyhedron polyhedron = object.ToPolyhedron();
		float3 endPont(end);
		LineSegment line(pos, endPont);
		float3 interesectPoint = polyhedron.ClosestPoint(line);
		intersectCoord.x = interesectPoint.x;
		intersectCoord.y = interesectPoint.y;
		intersectCoord.z = interesectPoint.z;
		return true;
	}
	return false;
}

void CBoundingBox::Draw(float x, float y, float z, float rotation) const
{
	float box[24] = {m_min[0], m_min[1], m_min[2],
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

void CBoundingBox::Scale()
{
	m_min[0] *= m_scale;
	m_min[1] *= m_scale;
	m_min[2] *= m_scale;
	m_max[0] *= m_scale;
	m_max[1] *= m_scale;
	m_max[2] *= m_scale;
}

void CBoundingCompound::AddChild(std::shared_ptr<IBounding> child)
{
	m_children.push_back(child);
}

void CBoundingCompound::Draw(float x, float y, float z, float rotation) const
{
	for(size_t i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->Draw(x, y, z, rotation);
	}
}

bool CBoundingCompound::IsIntersectsRay(float origin[3], float end[3], float x, float y, float z, float rotation, CVector3d & intersectCoord) const
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

void CBoundingBox::SetScale(float scale)
{ 
	m_scale = scale; 
}

float CBoundingBox::GetScale() const
{ 
	return m_scale; 
}

void CBoundingCompound::SetScale(float scale)
{
	for(size_t i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->SetScale(scale);
	}
}

float CBoundingCompound::GetScale() const
{ 
	if (m_children.size() >= 1)
	{
		return m_children[0]->GetScale();
	}
	return 1;
}

std::shared_ptr<IBounding> LoadBoundingFromFile(std::string const& path, float & scale)
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
			float min[3], max[3];
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

bool IsInteresect( CBoundingBox const& bounding1, float3 translate1, float angle1,  CBoundingBox const& bounding2, float3 translate2, float angle2 )
{
	OBB ob1 = GetOBB(bounding1, translate1, angle1);
	OBB ob2 = GetOBB(bounding2, translate2, angle2);
	return ob1.Intersects(ob2);
}

bool IsInteresect( CBoundingBox const& bounding1, float3 translate1, float angle1,  CBoundingCompound const& bounding2, float3 translate2, float angle2 )
{
	for (unsigned int i = 0; i < bounding2.GetChildCount(); i++)
	{
		if (IsInteresect( (IBounding*)&bounding1, translate1, angle1,  bounding2.GetChild(i), translate2, angle2 ))
		{
			return true;
		}
	}
	return false;
}

bool IsInteresect( CBoundingCompound const& bounding1, float3 translate1, float angle1,  CBoundingCompound const& bounding2, float3 translate2, float angle2 )
{
	for (unsigned int i = 0; i < bounding2.GetChildCount(); i++)
	{
		if (IsInteresect( (IBounding*)&bounding1, translate1, angle1,  bounding2.GetChild(i), translate2, angle2 ))
		{
			return true;
		}
	}
	return false;
}

bool IsInteresect( IBounding* bounding1, float3 translate1, float angle1,  IBounding* bounding2, float3 translate2, float angle2 )
{
	if (bounding1->GetTypeObject() == IBounding::BOX)
	{
		if (bounding2->GetTypeObject() == IBounding::BOX)
		{
			CBoundingBox box1 = *( dynamic_cast<CBoundingBox*>(bounding1) );
			CBoundingBox box2 = *( dynamic_cast<CBoundingBox*>(bounding2) );
			return IsInteresect( box1, translate1, angle1, box2, translate2, angle2);
		}
		else
		{
			CBoundingBox box1 = *( dynamic_cast<CBoundingBox*>(bounding1) );
			CBoundingCompound box2 = *( dynamic_cast<CBoundingCompound*>(bounding2) );
			return IsInteresect( box1, translate1, angle1, box2, translate2, angle2);
		}
	}
	else if (bounding1->GetTypeObject() == IBounding::COMPOUND)
	{
		if (bounding2->GetTypeObject() == IBounding::BOX)
		{
			CBoundingCompound box1 = *( dynamic_cast<CBoundingCompound*>(bounding1) );
			CBoundingBox box2 = *( dynamic_cast<CBoundingBox*>(bounding2) );
			return IsInteresect( box2, translate2, angle2, box1, translate1, angle1);
		}
		else
		{
			CBoundingCompound box1 = *( dynamic_cast<CBoundingCompound*>(bounding1) );
			CBoundingCompound box2 = *( dynamic_cast<CBoundingCompound*>(bounding2) );
			return IsInteresect( box1, translate1, angle1, box2, translate2, angle2);
		}
	}
}


AABB GetAABB(CBoundingBox const& bounding)
{
	math::float3 minVector(bounding.GetMin());
	math::float3 maxVector(bounding.GetMax());
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