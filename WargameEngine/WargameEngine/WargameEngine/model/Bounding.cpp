#include "Bounding.h"
#include <cstring>
#include <math.h>
#pragma warning( push )
#pragma warning( disable : 4458 4996 4244)
#include "Geometry/Ray.h"
#include "Geometry/Polyhedron.h"
#include "Geometry/LineSegment.h"
#include "Geometry/Line.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "Math/float3.h"
#include "Math/float3x3.h"

CBoundingBox::CBoundingBox(double min[3], double max[3]) 
	:m_scale(1.0)
{ 
	memcpy(m_min, min, sizeof(double) * 3); 
	memcpy(m_max, max, sizeof(double) * 3); 
}
template<class T>
float3 ToFloat3(T const& vec)
{
	return float3(static_cast<float>(vec[0]), static_cast<float>(vec[1]), static_cast<float>(vec[2]));
}

AABB GetAABB(CBoundingBox const& bounding)
{
	const double *min = bounding.GetMin();
	const double *max = bounding.GetMax();
	math::float3 minVector = ToFloat3(min);
	math::float3 maxVector = ToFloat3(max);
	return AABB(minVector, maxVector);
}

OBB GetOBB(CBoundingBox const& bounding, float3 const& translate, double angle)
{
	OBB res( GetAABB(bounding) );
	float z = res.r.z;
	res.Scale( res.CenterPoint(), static_cast<float>(bounding.GetScale()) );
	res.Translate(float3(0, 0, res.r.z - z));

	float fangle = static_cast<float>(angle);

	float3x3 rotateMatrix(
		cos(fangle), -sin(fangle), 0.0f,
		sin(fangle), cos(fangle), 0.0f,
		0.0f, 0.0f, 1.0f
	);
	res.Transform(rotateMatrix);
	res.Translate(translate);
	return res;
}

bool CBoundingBox::IsIntersectsRay(const double * origin, const double *end, double x, double y, double z, double rotation, CVector3d & intersectCoord) const
{
	float3 pos = ToFloat3(origin);
	float3 dir = ToFloat3(CVector3d(end) - CVector3d(origin));
	Line l(pos, dir);

	float3 objPos(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

	OBB object = GetOBB(*this, objPos, static_cast<float>(rotation));
	
	if ( object.Intersects(l) )
	{
		Polyhedron polyhedron = object.ToPolyhedron();
		float end1[3] = { static_cast<float>(end[0]), static_cast<float>(end[1]), static_cast<float>(end[2])};
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
	float3 tr1 = ToFloat3(translate1);
	float3 tr2 = ToFloat3(translate2);
	OBB ob1 = GetOBB(bounding1, tr1, angle1);
	OBB ob2 = GetOBB(bounding2, tr2, angle2);
	return ob1.Intersects(ob2);
}

bool IsInteresect(CBoundingBox const& bounding1, CVector3d const& translate1, double angle1, CBoundingCompound const& bounding2, CVector3d const& translate2, double angle2)
{
	for (size_t i = 0; i < bounding2.GetChildCount(); i++)
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
	for (size_t i = 0; i < bounding2.GetChildCount(); i++)
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
			if (b2c)
			{
				return IsInteresect(*b1, translate1, angle1, *b2c, translate2, angle2);
			}
			return false;
		}
	}
	else
	{
		const CBoundingCompound* b1c = dynamic_cast<const CBoundingCompound*>(bounding1);
		if (!b1c)
		{
			return false;
		}
		const CBoundingBox * b2 = dynamic_cast<const CBoundingBox*>(bounding2);
		if (b2)
		{
			return IsInteresect(*b2, translate2, angle2, *b1c, translate1, angle1);
		}
		else
		{
			const CBoundingCompound * b2c = dynamic_cast<const CBoundingCompound*>(bounding2);
			if (!b2c)
			{
				return false;
			}
			return IsInteresect(*b1c, translate1, angle1, *b2c, translate2, angle2);
		}
	}
}

void CBoundingCompound::AddChild(std::unique_ptr<IBounding> child)
{
	m_children.push_back(std::move(child));
}

bool CBoundingCompound::IsIntersectsRay(const double * origin, const double *end, double x, double y, double z, double rotation, CVector3d & intersectCoord) const
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

size_t CBoundingCompound::GetChildCount() const
{ 
	return m_children.size(); 
}

IBounding * CBoundingCompound::GetChild(size_t index)
{ 
	return m_children[index].get(); 
}

const IBounding * CBoundingCompound::GetChild(size_t index) const
{ 
	return m_children[index].get(); 
}

#pragma warning( pop )