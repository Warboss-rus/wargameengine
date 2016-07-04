#pragma once
#include <math.h>
#include <string>
#include <functional>
#include "../view/Vector3.h"
class IMoveLimiter
{
public:
	virtual void FixPosition(CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation) const = 0;
	virtual ~IMoveLimiter() {}
};

class CMoveLimiterCircle : public IMoveLimiter
{
public:
	CMoveLimiterCircle(double x, double y, double radius):m_x(x), m_y(y), m_radius(radius) {}
	void FixPosition(CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation) const;
private:
	double m_x;
	double m_y;
	double m_radius;
};

class CMoveLimiterRectangle : public IMoveLimiter
{
public:
	CMoveLimiterRectangle(double x1, double y1, double x2, double y2):m_minX((x1 < x2)?x1:x2), m_maxX((x1 > x2)?x1:x2), m_minY((y1 < y2)?y1:y2), m_maxY((y1 > y2)?y1:y2) {}
	void FixPosition(CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation) const;
private:
	double m_minX;
	double m_maxX;
	double m_minY;
	double m_maxY;
};

class CMoveLimiterStatic : public IMoveLimiter
{
public:
	CMoveLimiterStatic(double x, double y, double z, double rotation):m_x(x), m_y(y), m_z(z), m_rotation(rotation) {}
	void FixPosition(CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation) const;
private:
	double m_x;
	double m_y;
	double m_z;
	double m_rotation;
};

class CMoveLimiterTiles : public IMoveLimiter
{
public:
	void FixPosition(CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation) const;
};

class CCustomMoveLimiter : public IMoveLimiter
{
public:
	typedef std::function<void(CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation)> CustomMoveLimiterHandler;
	CCustomMoveLimiter(CustomMoveLimiterHandler const& function);
	void FixPosition(CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation) const;
private:
	CustomMoveLimiterHandler m_function;
};