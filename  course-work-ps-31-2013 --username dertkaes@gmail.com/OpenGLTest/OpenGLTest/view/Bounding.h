#pragma once

#include <memory>
#include <vector>
#include "Vector3.h"
#include "../lib/src/Geometry/OBB.h"
#include "../lib/src/Geometry/AABB.h"
#include "../lib/src/Math/float3.h"
#include "../lib/src/Math/float3x3.h"

class CBoundingBox;
class CBoundingCompound;

class IBounding
{
public:
	enum TypeBounding
	{
		BOX,
		COMPOUND
	};
	virtual bool IsIntersectsRay(float origin[3], float end[3], float x, float y, float z, float rotation, CVector3d & intersectCoord) const = 0;
	virtual void Draw(float x, float y, float z, float rotation) const = 0;
	virtual void SetScale(float scale) = 0;
	virtual float GetScale() const = 0;
	virtual TypeBounding GetTypeObject() const = 0;
	virtual ~IBounding() {}
};

class CBoundingBox : public IBounding
{
public:
	CBoundingBox(float min[3], float max[3]);
	bool IsIntersectsRay(float origin[3], float end[3], float x, float y, float z, float rotation, CVector3d & intersectCoord) const;
	bool OverlapsLineSegment(CVector3d const& mid,  CVector3d const& dir,  const float hl) const;
	void Draw(float x, float y, float z, float rotation) const;
	void SetScale(float scale);
	void Scale();
	float GetScale() const;
	float* GetMin()
	{
		return &m_min[0];
	}
	float* GetMax()
	{
		return &m_max[0];
	}
	float3 GetMin() const
	{
		float minB[3] = {m_min[0], m_min[1], m_min[2]};
		return float3(minB);
	}
	float3 GetMax() const 
	{
		float maxB[3] = {m_max[0], m_max[1], m_max[2]};
		return float3(maxB);
	}
	TypeBounding GetTypeObject() const
	{
		return TypeBounding::BOX;
	}
private:
	float m_min[3];
	float m_max[3];
	float m_scale;
};

class CBoundingCompound : public IBounding
{
public:
	void AddChild(std::shared_ptr<IBounding> child);
	bool IsIntersectsRay(float origin[3], float end[3], float x, float y, float z, float rotation, CVector3d & intersectCoord) const;
	void Draw(float x, float y, float z, float rotation) const;
	void SetScale(float scale);
	float GetScale() const;
	TypeBounding GetTypeObject() const
	{
		return TypeBounding::COMPOUND;
	}
	unsigned int GetChildCount() const { return m_children.size(); }
	IBounding * GetChild(unsigned int index)const { return m_children[index].get(); }
private:
	std::vector<std::shared_ptr<IBounding>> m_children;
};

std::shared_ptr<IBounding> LoadBoundingFromFile(std::string const& path, float & scale);

AABB GetAABB(CBoundingBox const& bounding);
OBB GetOBB(const CBoundingBox const& bounding, float3 translate, float angle);

bool IsInteresect( IBounding* bounding1, float3 translate1, float angle1,  IBounding* bounding2, float3 translate2, float angle2 );
bool IsInteresect( CBoundingCompound const& bounding1, float3 translate1, float angle1,  CBoundingCompound const& bounding2, float3 translate2, float angle2 );
bool IsInteresect( CBoundingBox const& bounding1, float3 translate1, float angle1,  CBoundingCompound const& bounding2, float3 translate2, float angle2 );
bool IsInteresect( CBoundingBox const& bounding1, float3 translate1, float angle1,  CBoundingBox const& bounding2, float3 translate2, float angle2 );
