#include "Ruler.h"
#include <GL\glut.h>
#include <math.h>
#include "SelectionTools.h"
#include <string>

void CRuler::SetBegin(int windowBeginX, int windowBeginY)
{
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
	char str[10];
	sprintf(str, "%0.2f", GetDistance());
	PrintText(m_worldEndX, m_worldEndY, str);
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

void CRuler::PrintText( double x, double y, char *st) const
{
	int l,i;

	l=strlen( st ); // see how many characters are in text string.
	glRasterPos2d( x, y); // location to start printing text
	for( i=0; i < l; i++) // loop until i is greater then l
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st[i]); // Print a character on the screen
	}
}