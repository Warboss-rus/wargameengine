#include <math.h>
#pragma once
class IMoveLimiter
{
public:
	virtual void FixPosition(float & x, float & y, float & z, float & rotation) const = 0;
	virtual ~IMoveLimiter() {}
};

class CMoveLimiterCircle : public IMoveLimiter
{
public:
	CMoveLimiterCircle(float x, float y, float radius):m_x(x), m_y(y), m_radius(radius) {}
	void FixPosition(float & x, float & y, float & z, float & rotation) const;
private:
	float m_x;
	float m_y;
	float m_radius;
};

class CMoveLimiterRectangle : public IMoveLimiter
{
public:
	CMoveLimiterRectangle(float x1, float y1, float x2, float y2):m_minX((x1 < x2)?x1:x2), m_maxX((x1 > x2)?x1:x2), m_minY((y1 < y2)?y1:y2), m_maxY((y1 > y2)?y1:y2) {}
	void FixPosition(float & x, float & y, float & z, float & rotation) const;
private:
	float m_minX;
	float m_minY;
	float m_maxX;
	float m_maxY;
};

class CMoveLimiterStatic : public IMoveLimiter
{
public:
	CMoveLimiterStatic(float x, float y, float z, float rotation):m_x(x), m_y(y), m_z(z), m_rotation(rotation) {}
	void FixPosition(float & x, float & y, float & z, float & rotation) const;
private:
	float m_x;
	float m_y;
	float m_z;
	float m_rotation;
};