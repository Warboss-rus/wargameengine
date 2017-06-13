#include "MovementLimiter.h"

namespace wargameEngine
{
namespace controller
{
bool MoveLimiterRectangle::FixPosition(CVector3f& position, CVector3f& /*rotations*/, const CVector3f& /*oldPosition*/, const CVector3f& /*oldRotations*/) const
{
	if (position.x < m_minX)
		position.x = m_minX;
	if (position.x > m_maxX)
		position.x = m_maxX;
	if (position.y < m_minY)
		position.y = m_minY;
	if (position.y > m_maxY)
		position.y = m_maxY;
	return position.x >= m_minX && position.x <= m_maxX && position.y >= m_minY && position.y <= m_maxY;
}

bool MoveLimiterCircle::FixPosition(CVector3f& position, CVector3f& /*rotations*/, const CVector3f& /*oldPosition*/, const CVector3f& /*oldRotations*/) const
{
	if (sqrt((position.x - m_x) * (position.x - m_x) + (position.y - m_y) * (position.y - m_y)) > m_radius)
	{
		float angle = atan2(position.y - m_y, position.x - m_x);
		position.x = m_x + m_radius * cos(angle);
		position.y = m_y + m_radius * sin(angle);
		return false;
	}
	return true;
}

bool MoveLimiterStatic::FixPosition(CVector3f& position, CVector3f& rotation, const CVector3f& oldPosition, const CVector3f& oldRotation) const
{
	position = oldPosition;
	rotation = oldRotation;
	return false;
}

bool MoveLimiterTiles::FixPosition(CVector3f& position, CVector3f& /*rotation*/, const CVector3f& /*oldPosition*/, const CVector3f& /*oldRotation*/) const
{
	position.x = floor(position.x);
	position.y = floor(position.y);
	position.z = floor(position.z);
	return false;
}

CustomMoveLimiter::CustomMoveLimiter(CustomMoveLimiterHandler const& function)
	: m_function(function)
{
}

bool CustomMoveLimiter::FixPosition(CVector3f& position, CVector3f& rotation, const CVector3f& oldPosition, const CVector3f& oldRotation) const
{
	return m_function(position, rotation, oldPosition, oldRotation);
}
}
}