#include "Ruler.h"
#include <GL\glut.h>
#include <math.h>
#include "SelectionTools.h"

void WindowCoordsToWorldCoords(int windowX, int windowY, double & worldX, double & worldY)
{
	double startx, starty, startz, endx, endy, endz;
	WindowCoordsToWorldVector(windowX, windowY, startx, starty, startz, endx, endy, endz);
	double a = (-startz) / (endz - startz);
	worldX = a * (endx - startx) + startx;
	worldY = a * (endy - starty) + starty;
}

void CRuler::SetBegin(int windowBeginX, int windowBeginY)
{
	if(m_isVisible) return;
	WindowCoordsToWorldCoords(windowBeginX, windowBeginY, m_worldBeginX, m_worldBeginY);
	m_worldEndX = m_worldBeginX;
	m_worldEndY = m_worldBeginY;
	m_isVisible = true;
}

void CRuler::SetEnd(int windowEndX, int windowEndY)
{
	WindowCoordsToWorldCoords(windowEndX, windowEndY, m_worldEndX, m_worldEndY);
}

void CRuler::Draw() const
{
	if(!m_isVisible) return;
	glDisable(GL_DEPTH_TEST);
	glColor3d(255.0, 255.0, 0.0);
	glBegin(GL_LINES);
	glVertex3d(m_worldBeginX, m_worldBeginY, 0.0);
	glVertex3d(m_worldEndX, m_worldEndY, 0.0);
	glEnd();
	glColor3d(255.0, 255.0, 255.0);
	glEnable(GL_DEPTH_TEST);
}

double CRuler::GetDistance() const
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