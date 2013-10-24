#include <memory>
#include <vector>
class IBounding
{
public:
	virtual bool IsIntersectsRay(double origin[3], double dir[3], double x, double y, double z, double rotation) const = 0;
	virtual void Draw(double x, double y, double z, double rotation) const = 0;
	virtual ~IBounding() {}
};

class CBoundingBox : public IBounding
{
public:
	CBoundingBox(double min[3], double max[3]);
	bool IsIntersectsRay(double origin[3], double dir[3], double x, double y, double z, double rotation) const;
	void Draw(double x, double y, double z, double rotation) const;
private:
	double m_min[3];
	double m_max[3];
};

class CBoundingCompound : public IBounding
{
public:
	void AddChild(std::shared_ptr<IBounding> child);
	bool IsIntersectsRay(double origin[3], double dir[3], double x, double y, double z, double rotation) const;
	void Draw(double x, double y, double z, double rotation) const;
private:
	std::vector<std::shared_ptr<IBounding>> m_children;
};

std::shared_ptr<IBounding> LoadBoundingFromFile(std::string const& path);