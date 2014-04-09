#include "UIScrollBar.h"
#include <GL\glut.h>

const int g_width = 20;

void CUIScrollBar::Update(int size, int contentSize, int width)
{
	m_size = size;
	m_contentSize = contentSize;
	m_width = width;
	if(m_position > contentSize - size)
		m_position = (contentSize - size < 0)?0:contentSize - size;
}

void CUIScrollBar::Draw() const
{
	float scrollSize = (float)m_size / (float)m_contentSize;
	if(scrollSize >= 1)
		return;
	glColor3ub(128, 128, 128);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(m_width - g_width, 0);
	glVertex2f(m_width, 0);
	glVertex2f(m_width - g_width, m_size);
	glVertex2f(m_width, m_size);
	glEnd();
	glColor3ub(200, 200, 200);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(m_width - g_width, m_position);
	glVertex2f(m_width, m_position);
	glVertex2f(m_width - g_width, m_position + m_size * scrollSize);
	glVertex2f(m_width, m_position + m_size * scrollSize);
	glEnd();
}

bool CUIScrollBar::LeftMouseButtonDown(int x, int y)
{
	float scrollSize = (float)m_size / (float)m_contentSize;
	if(scrollSize >= 1)
		return false;
	m_mousePos = y;
	return true;
}

bool CUIScrollBar::LeftMouseButtonUp(int x, int y)
{
	float scrollSize = (float)m_size / (float)m_contentSize;
	if(scrollSize >= 1)
		return false;
	m_position += y - m_mousePos;
	if(m_position < 0) m_position = 0;
	if(m_position + m_size * scrollSize > m_size) m_position = m_size - m_size * scrollSize;
	return true;
}

bool CUIScrollBar::IsOnElement(int x, int y) const
{
	if((float)m_size / (float)m_contentSize >= 1)
		return false;
	return (x >= m_width - g_width && x <= m_width && y >= 0 && y < m_size);
}