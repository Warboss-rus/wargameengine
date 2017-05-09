#pragma once
#include "../view/Vector3.h"
#include <functional>
#include <math.h>
#include <string>

class IMoveLimiter
{
public:
	virtual bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const = 0;
	virtual ~IMoveLimiter() {}
};

class CMoveLimiterCircle : public IMoveLimiter
{
public:
	CMoveLimiterCircle(float x, float y, float radius)
		: m_x(x)
		, m_y(y)
		, m_radius(radius)
	{
	}
	bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const override;

private:
	float m_x;
	float m_y;
	float m_radius;
};

class CMoveLimiterRectangle : public IMoveLimiter
{
public:
	CMoveLimiterRectangle(float x1, float y1, float x2, float y2)
		: m_minX((x1 < x2) ? x1 : x2)
		, m_maxX((x1 > x2) ? x1 : x2)
		, m_minY((y1 < y2) ? y1 : y2)
		, m_maxY((y1 > y2) ? y1 : y2)
	{
	}
	bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const override;

private:
	float m_minX;
	float m_maxX;
	float m_minY;
	float m_maxY;
};

class CMoveLimiterStatic : public IMoveLimiter
{
public:
	CMoveLimiterStatic() {}
	bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const override;
};

class CMoveLimiterTiles : public IMoveLimiter
{
public:
	bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const override;
};

class CCustomMoveLimiter : public IMoveLimiter
{
public:
	typedef std::function<bool(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations)> CustomMoveLimiterHandler;
	CCustomMoveLimiter(CustomMoveLimiterHandler const& function);
	bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const override;

private:
	CustomMoveLimiterHandler m_function;
};