#include "Ruler.h"
#include "view/gl.h"
#include <math.h>
#include <string>

CRuler::CRuler() 
{ 
	Hide(); 
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

void CRuler::Draw() const
{
	if(!m_isVisible) return;
	glColor3d(255.0, 255.0, 0.0);
	glBegin(GL_LINES);
	glVertex3d(m_worldBeginX, m_worldBeginY, 0.0);
	glVertex3d(m_worldEndX, m_worldEndY, 0.0);
	glEnd();
	glColor3d(255.0, 255.0, 255.0);
	char str[10];
	sprintf(str, "%0.2f", GetDistance());
	PrintText(m_worldEndX, m_worldEndY, str);
}

float CRuler::GetDistance() const
{
	float deltaX = m_worldEndX - m_worldBeginX;
	float deltaY = m_worldEndY - m_worldBeginY;
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

void CRuler::PrintText( float x, float y, const char *st) const
{
	int l,i;

	l=strlen( st ); // see how many characters are in text string.
	glRasterPos2d( x, y); // location to start printing text
	for( i=0; i < l; i++) // loop until i is greater then l
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st[i]); // Print a character on the screen
	}
}