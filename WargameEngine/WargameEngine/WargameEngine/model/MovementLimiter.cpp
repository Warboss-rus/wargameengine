#include "MovementLimiter.h"

void CMoveLimiterRectangle::FixPosition(CVector3d & position, double & /*rotation*/, const CVector3d & /*oldPosition*/, double /*oldRotation*/) const
{ 
	if (position.x < m_minX) position.x = m_minX;
	if (position.x > m_maxX) position.x = m_maxX;
	if (position.y < m_minY) position.y = m_minY;
	if (position.y > m_maxY) position.y = m_maxY;
}

void CMoveLimiterCircle::FixPosition(CVector3d & position, double & /*rotation*/, const CVector3d & /*oldPosition*/, double /*oldRotation*/) const
{
	if (sqrt((position.x - m_x) * (position.x - m_x) + (position.y - m_y) * (position.y - m_y)) > m_radius)
	{
		double angle = atan2(position.y - m_y, position.x - m_x);
		position.x = m_x + m_radius * cos(angle);
		position.y = m_y + m_radius * sin(angle);
	}
}

void CMoveLimiterStatic::FixPosition(CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation) const
{
	position = oldPosition;
	rotation = oldRotation;
}

void CMoveLimiterTiles::FixPosition(CVector3d & position, double & /*rotation*/, const CVector3d & /*oldPosition*/, double /*oldRotation*/) const
{
	position.x = floor(position.x);
	position.y = floor(position.y);
	position.z = floor(position.z);
}

CCustomMoveLimiter::CCustomMoveLimiter(CustomMoveLimiterHandler const& function) :m_function(function)
{
}

void CCustomMoveLimiter::FixPosition(CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation) const
{
	m_function(position, rotation, oldPosition, oldRotation);
}