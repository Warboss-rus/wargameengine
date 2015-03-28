#include "Bounding.h"
#include <cstring>
#include <math.h>
#pragma warning( push )
#pragma warning( disable : 4458)
#include "Geometry/Ray.h"
#include "Geometry/Polyhedron.h"
#include "Geometry/LineSegment.h"
#include "Geometry/Line.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "Math/float3.h"
#include "Math/float3x3.h"
#pragma warning( pop )

CBoundingBox::CBoundingBox(double min[3], double max[3]) 
{ 
	memcpy(m_min, min, sizeof(double) * 3); 
	memcpy(m_max, max, sizeof(double) * 3); 
}

AABB GetAABB(CBoundingBox const& bounding)
{
	const double *min = bounding.GetMin();
	const double *max = bounding.GetMax();
	float min1[3] = {min[0], min[1], min[2]};
	float max1[3] = {max[0], max[1], max[2]};
	math::float3 minVector(min1);
	math::float3 maxVector(max1);
	return AABB(minVector, maxVector);
}

OBB GetOBB(CBoundingBox const& bounding, float3 translate, double angle)
{
	OBB res( GetAABB(bounding) );
	float z = res.r.z;
	res.Scale( res.CenterPoint(), bounding.GetScale() );
	res.Translate(float3(0, 0, res.r.z - z));

	float3x3 rotateMatrix(
		cos(angle), -sin(angle), 0.0f,
		sin(angle), cos(angle), 0.0f,
		0.0f, 0.0f, 1.0f
	);
	res.Transform(rotateMatrix);
	res.Translate(translate);
	return res;
}

bool CBoundingBox::IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const
{
	float origin1[3] = {origin[0], origin[1], origin[2]};
	float3 pos(origin1);
	float3 dir(end[0] - origin[0], end[1] - origin[1], end[2] - origin[2]);
	Line l(pos, dir);

	OBB object = GetOBB(*this, float3(x, y, z), rotation);
	
	if ( object.Intersects(l) )
	{
		Polyhedron polyhedron = object.ToPolyhedron();
		float end1[3] = {end[0], end[1], end[2]};
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

const double* CBoundingBox::GetMin() const
{
	return &m_min[0];
}

const double* CBoundingBox::GetMax() const
{
	return &m_max[0];
}

const double CBoundingBox::GetScale() const
{ 
	return m_scale; 
}

bool IsInteresect(CBoundingBox const& bounding1, CVector3d const& translate1, double angle1, CBoundingBox const& bounding2, CVector3d const& translate2, double angle2)
{
	float3 tr1(translate1[0], translate1[1], translate1[2]);
	float3 tr2(translate2[0], translate2[1], translate2[2]);
	OBB ob1 = GetOBB(bounding1, tr1, angle1);
	OBB ob2 = GetOBB(bounding2, tr2, angle2);
	return ob1.Intersects(ob2);
}

bool IsInteresect(CBoundingBox const& bounding1, CVector3d const& translate1, double angle1, CBoundingCompound const& bounding2, CVector3d const& translate2, double angle2)
{
	for (unsigned int i = 0; i < bounding2.GetChildCount(); i++)
	{
		if (IsInteresect(&bounding1, translate1, angle1, bounding2.GetChild(i), translate2, angle2))
		{
			return true;
		}
	}
	return false;
}

bool IsInteresect(CBoundingCompound const& bounding1, CVector3d const& translate1, double angle1, CBoundingCompound const& bounding2, CVector3d const& translate2, double angle2)
{
	for (unsigned int i = 0; i < bounding2.GetChildCount(); i++)
	{
		if (IsInteresect((IBounding*)&bounding1, translate1, angle1, bounding2.GetChild(i), translate2, angle2))
		{
			return true;
		}
	}
	return false;
}

bool IsInteresect(const IBounding* bounding1, CVector3d const& translate1, double angle1, const IBounding* bounding2, CVector3d const& translate2, double angle2)
{
	const CBoundingBox * b1 = dynamic_cast<const CBoundingBox*>(bounding1);
	if (b1)
	{
		const CBoundingBox * b2 = dynamic_cast<const CBoundingBox*>(bounding2);
		if (b2)
		{
			return IsInteresect(*b1, translate1, angle1, *b2, translate2, angle2);
		}
		else
		{
			const CBoundingCompound* b2c = dynamic_cast<const CBoundingCompound*>(bounding2);
			return IsInteresect(*b1, translate1, angle1, *b2c, translate2, angle2);
		}
	}
	else
	{
		const CBoundingCompound* b1c = dynamic_cast<const CBoundingCompound*>(bounding1);
		const CBoundingBox * b2 = dynamic_cast<const CBoundingBox*>(bounding2);
		if (b2)
		{
			return IsInteresect(*b2, translate2, angle2, *b1c, translate1, angle1);
		}
		else
		{
			const CBoundingCompound * b2c = dynamic_cast<const CBoundingCompound*>(bounding2);
			return IsInteresect(*b1c, translate1, angle1, *b2c, translate2, angle2);
		}
	}
}

void CBoundingCompound::AddChild(std::unique_ptr<IBounding> child)
{
	m_children.push_back(std::move(child));
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

unsigned int CBoundingCompound::GetChildCount() const
{ 
	return m_children.size(); 
}

IBounding * CBoundingCompound::GetChild(unsigned int index)
{ 
	return m_children[index].get(); 
}

const IBounding * CBoundingCompound::GetChild(unsigned int index) const
{ 
	return m_children[index].get(); 
}