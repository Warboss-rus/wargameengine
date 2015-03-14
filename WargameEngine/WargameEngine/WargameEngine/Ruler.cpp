#include "Ruler.h"
#include "view/GameView.h"
#include <GL/glut.h>
#include <math.h>
#include <string>
#include <cstring>

bool CRuler::m_isVisible = false;
double CRuler::m_worldBeginX = 0.0;
double CRuler::m_worldBeginY = 0.0;
double CRuler::m_worldEndX = 0.0;
double CRuler::m_worldEndY = 0.0;

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

void CRuler::Draw()
{
	if(!m_isVisible) return;
	CGameView::GetInstance().lock()->DrawLine(m_worldBeginX, m_worldBeginY, 0.0, m_worldEndX, m_worldEndY, 0.0, 255, 255, 0);
	char str[10];
	sprintf(str, "%0.2f", GetDistance());
	CGameView::GetInstance().lock()->DrawText3D(m_worldEndX, m_worldEndY, 0.0, str);
}

double CRuler::GetDistance()
{
	double deltaX = m_worldEndX - m_worldBeginX;
	double deltaY = m_worldEndY - m_worldBeginY;
	return sqrt(deltaX * deltaX + deltaY * deltaY);
}

void CRuler::Hide()
{
	m_isVisible = false;
	m_worldBeginX = 0.0;
	m_worldBeginY = 0.0;
	m_worldEndX = 0.0;
	m_worldEndY = 0.0;
}