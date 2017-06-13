#include "Ruler.h"
#include <math.h>
#include <string>
#include <cstring>

namespace wargameEngine
{
namespace view
{
Ruler::Ruler()
	: m_enabled(false)
	, m_isVisible(false)
	, m_worldBeginX(0.0f), m_worldEndX(0.0f), m_worldBeginY(0.0f), m_worldEndY(0.0f)
{

}

void Ruler::Enable()
{
	m_enabled = true;
}

void Ruler::Disable()
{
	m_enabled = false;
}

void Ruler::SetBegin(float x, float y)
{
	m_worldBeginX = x;
	m_worldBeginY = y;
	m_worldEndX = x;
	m_worldEndY = y;
	m_isVisible = true;
}

void Ruler::SetEnd(float x, float y)
{
	m_worldEndX = x;
	m_worldEndY = y;
}

double Ruler::GetDistance() const
{
	double deltaX = m_worldEndX - m_worldBeginX;
	double deltaY = m_worldEndY - m_worldBeginY;
	return sqrt(deltaX * deltaX + deltaY * deltaY);
}

bool Ruler::IsVisible() const
{
	return m_isVisible;
}

bool Ruler::IsEnabled() const
{
	return m_enabled;
}

CVector3f Ruler::GetBegin() const
{
	return { m_worldBeginX, m_worldBeginY, 0.0f };
}

CVector3f Ruler::GetEnd() const
{
	return{ m_worldEndX, m_worldEndY, 0.0f };
}

void Ruler::Hide()
{
	m_isVisible = false;
	m_worldBeginX = 0.0f;
	m_worldBeginY = 0.0f;
	m_worldEndX = 0.0f;
	m_worldEndY = 0.0f;
	m_enabled = false;
}
}
}