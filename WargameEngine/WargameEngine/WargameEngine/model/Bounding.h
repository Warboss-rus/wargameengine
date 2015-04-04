#pragma once
#include <memory>
#include <vector>
#include "../view/Vector3.h"


class IBounding
{
public:
	virtual bool IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const = 0;
	virtual void SetScale(double scale) = 0;
	virtual ~IBounding() {}
};

class CBoundingBox : public IBounding
{
public:
	CBoundingBox(double min[3], double max[3]);
	bool IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const override;
	void SetScale(double scale) override { m_scale = scale; }
	const double* GetMin() const;
	const double* GetMax() const;
	const double GetScale() const;
private:
	bool OverlapsLineSegment(CVector3d const& mid, CVector3d const& dir, const double hl) const;
	double m_min[3];
	double m_max[3];
	double m_scale;
};

class CBoundingCompound : public IBounding
{
public:
	void AddChild(std::unique_ptr<IBounding> child);
	bool IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const override;
	void SetScale(double scale) override;
	size_t GetChildCount() const;
	IBounding * GetChild(unsigned int index);
	const IBounding * GetChild(unsigned int index) const;
private:
	std::vector<std::unique_ptr<IBounding>> m_children;
};

bool IsInteresect(const IBounding* bounding1, CVector3d const& translate1, double angle1, const IBounding* bounding2, CVector3d const& translate2, double angle2);