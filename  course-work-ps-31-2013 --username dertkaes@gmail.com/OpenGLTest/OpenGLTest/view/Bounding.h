#include <memory>
#include <vector>
#include "Vector3.h"
class IBounding
{
public:
	virtual bool IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const = 0;
	virtual void Draw(double x, double y, double z, double rotation) const = 0;
	virtual void SetScale(double scale) = 0;
	virtual ~IBounding() {}
};

class CBoundingBox : public IBounding
{
public:
	CBoundingBox(double min[3], double max[3]);
	bool IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const;
	bool OverlapsLineSegment(CVector3d const& mid,  CVector3d const& dir,  const double hl) const;
	void Draw(double x, double y, double z, double rotation) const;
	void SetScale(double scale) { m_scale = scale; }
private:
	double m_min[3];
	double m_max[3];
	double m_scale;
};

class CBoundingCompound : public IBounding
{
public:
	void AddChild(std::shared_ptr<IBounding> child);
	bool IsIntersectsRay(double origin[3], double end[3], double x, double y, double z, double rotation, CVector3d & intersectCoord) const;
	void Draw(double x, double y, double z, double rotation) const;
	void SetScale(double scale);
private:
	std::vector<std::shared_ptr<IBounding>> m_children;
};

std::shared_ptr<IBounding> LoadBoundingFromFile(std::string const& path, double & scale);