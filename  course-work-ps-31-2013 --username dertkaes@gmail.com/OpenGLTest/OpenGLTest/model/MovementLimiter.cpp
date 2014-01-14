#include "MovementLimiter.h"

void CMoveLimiterRectangle::FixPosition(float & x, float & y, float & z, float & rotation) const
{ 
	if(x < m_minX) x = m_minX; 
	if(x > m_maxX) x = m_maxX;
	if(y < m_minY) y = m_minY;
	if(y > m_maxY) y = m_maxY;
}

void CMoveLimiterCircle::FixPosition(float & x, float & y, float & z, float & rotation) const
{
	if(sqrt((x - m_x) * (x - m_x) + (y - m_y) * (y - m_y)) > m_radius)
	{
		float angle = atan2(y - m_y, x - m_x);
		x = m_x + m_radius * cos(angle);
		y = m_y + m_radius * sin(angle);
	}
}

void CMoveLimiterStatic::FixPosition(float & x, float & y, float & z, float & rotation) const
{
	x = m_x;
	y = m_y;
	z = m_z;
	rotation = m_rotation;
}