#include "MovementLimiter.h"
#include "../controller/LUAScriptHandler.h"

void CMoveLimiterRectangle::FixPosition(double & x, double & y, double & z, double & rotation) const
{ 
	if(x < m_minX) x = m_minX; 
	if(x > m_maxX) x = m_maxX;
	if(y < m_minY) y = m_minY;
	if(y > m_maxY) y = m_maxY;
}

void CMoveLimiterCircle::FixPosition(double & x, double & y, double & z, double & rotation) const
{
	if(sqrt((x - m_x) * (x - m_x) + (y - m_y) * (y - m_y)) > m_radius)
	{
		double angle = atan2(y - m_y, x - m_x);
		x = m_x + m_radius * cos(angle);
		y = m_y + m_radius * sin(angle);
	}
}

void CMoveLimiterStatic::FixPosition(double & x, double & y, double & z, double & rotation) const
{
	x = m_x;
	y = m_y;
	z = m_z;
	rotation = m_rotation;
}

void CMoveLimiterTiles::FixPosition(double & x, double & y, double & z, double & rotation) const
{
	x = floor(x);
	y = floor(y);
	z = floor(z);
}

CCustomMoveLimiter::CCustomMoveLimiter(std::string const& function) :m_function(function)
{
}

void CCustomMoveLimiter::FixPosition(double & x, double & y, double & z, double & rotation) const
{
	CLUAScript::CallFunctionReturn4(m_function, x, y, z, rotation, x, y, z, rotation);
}