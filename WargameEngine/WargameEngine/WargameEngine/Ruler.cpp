#include "Ruler.h"
#include <math.h>
#include <string>
#include <cstring>

CRuler::CRuler()
	: m_enabled(false)
	, m_isVisible(false)
	, m_worldBeginX(0.0f), m_worldEndX(0.0f), m_worldBeginY(0.0f), m_worldEndY(0.0f)
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

void CRuler::SetBegin(float x, float y)
{
	m_worldBeginX = x;
	m_worldBeginY = y;
	m_worldEndX = x;
	m_worldEndY = y;
	m_isVisible = true;
}

void CRuler::SetEnd(float x, float y)
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

CVector3f CRuler::GetBegin() const
{
	return { m_worldBeginX, m_worldBeginY, 0.0f };
}

CVector3f CRuler::GetEnd() const
{
	return{ m_worldEndX, m_worldEndY, 0.0f };
}

void CRuler::Hide()
{
	m_isVisible = false;
	m_worldBeginX = 0.0f;
	m_worldBeginY = 0.0f;
	m_worldEndX = 0.0f;
	m_worldEndY = 0.0f;
	m_enabled = false;
}