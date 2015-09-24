#include "Ruler.h"
#include "view/GameView.h"
#include <math.h>
#include <string>
#include <cstring>

CRuler::CRuler()
	:m_enabled(false)
	, m_isVisible(false)
{

}

void CRuler::Enable()
{
	m_enabled = true;
}

void CRuler::Disable()
{
	m_enabled = false;
}

void CRuler::SetBegin(double x, double y)
{
	m_worldBeginX = x;
	m_worldBeginY = y;
	m_worldEndX = x;
	m_worldEndY = y;
	m_isVisible = true;
}

void CRuler::SetEnd(double x, double y)
{
	m_worldEndX = x;
	m_worldEndY = y;
}

double CRuler::GetDistance() const
{
	double deltaX = m_worldEndX - m_worldBeginX;
	double deltaY = m_worldEndY - m_worldBeginY;
	return sqrt(deltaX * deltaX + deltaY * deltaY);
}

bool CRuler::IsVisible() const
{
	return m_isVisible;
}

bool CRuler::IsEnabled() const
{
	return m_enabled;
}

CVector3d CRuler::GetBegin() const
{
	return { m_worldBeginX, m_worldBeginY, 0.0 };
}

CVector3d CRuler::GetEnd() const
{
	return{ m_worldEndX, m_worldEndY, 0.0 };
}

void CRuler::Hide()
{
	m_isVisible = false;
	m_worldBeginX = 0.0;
	m_worldBeginY = 0.0;
	m_worldEndX = 0.0;
	m_worldEndY = 0.0;
	m_enabled = false;
}