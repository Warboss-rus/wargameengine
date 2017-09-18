#pragma once
#include "../view/Vector3.h"
#include <functional>
#include <math.h>
#include <string>

namespace wargameEngine
{
namespace controller
{
class IMoveLimiter
{
public:
	virtual bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const = 0;
	virtual ~IMoveLimiter() {}
};

class MoveLimiterCircle : public IMoveLimiter
{
public:
	MoveLimiterCircle(float x, float y, float radius)
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

class MoveLimiterRectangle : public IMoveLimiter
{
public:
	MoveLimiterRectangle(float x1, float y1, float x2, float y2)
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

class MoveLimiterStatic : public IMoveLimiter
{
public:
	MoveLimiterStatic() {}
	bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const override;
};

class MoveLimiterTiles : public IMoveLimiter
{
public:
	bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const override;
};

class CustomMoveLimiter : public IMoveLimiter
{
public:
	typedef std::function<bool(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations)> CustomMoveLimiterHandler;
	CustomMoveLimiter(CustomMoveLimiterHandler const& function);
	bool FixPosition(CVector3f& position, CVector3f& rotations, const CVector3f& oldPosition, const CVector3f& oldRotations) const override;

private:
	CustomMoveLimiterHandler m_function;
};
}
}