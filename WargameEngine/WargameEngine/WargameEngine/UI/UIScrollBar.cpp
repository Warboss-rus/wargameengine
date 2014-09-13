#include "UIScrollBar.h"
#include "../gl.h"
#include "../view/TextureManager.h"

void CUIScrollBar::Update(int size, int contentSize, int width, int step)
{
	m_size = size;
	m_contentSize = contentSize;
	m_width = width;
	m_position = 0;
	m_step = step;
}

void CUIScrollBar::Draw() const
{
	if (m_size >= m_contentSize) return;
	int scrollBegin = m_theme.sbar.buttonHeight;
	int scrollEnd = m_size - m_theme.sbar.buttonHeight;
	int scrollSize = (scrollEnd - scrollBegin) * ((float)m_size / (float)m_contentSize);
	glColor3ub(128, 128, 128);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(m_width - m_theme.sbar.width, scrollBegin);
	glVertex2f(m_width, scrollBegin);
	glVertex2f(m_width - m_theme.sbar.width, scrollEnd);
	glVertex2f(m_width, scrollEnd);
	glEnd();
	glColor3ub(200, 200, 200);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2f(m_width - m_theme.sbar.width, scrollBegin + m_position);
	glVertex2f(m_width, scrollBegin + m_position);
	glVertex2f(m_width - m_theme.sbar.width, scrollBegin + m_position + scrollSize);
	glVertex2f(m_width, scrollBegin + m_position + scrollSize);
	glEnd();
	CTextureManager::GetInstance()->SetTexture(m_theme.texture);
	const float * texcoords = (m_buttonPressed) ? m_theme.sbar.pressedTexCoord : m_theme.sbar.texCoord;
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(texcoords[0], texcoords[1]);
	glVertex2f(m_width - m_theme.sbar.width, 0.0f);
	glTexCoord2f(texcoords[2], texcoords[1]);
	glVertex2f(m_width, 0.0f);
	glTexCoord2f(texcoords[0], texcoords[3]);
	glVertex2f(m_width - m_theme.sbar.width, m_theme.sbar.buttonHeight);
	glTexCoord2f(texcoords[2], texcoords[3]);
	glVertex2f(m_width, m_theme.sbar.buttonHeight);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(texcoords[0], texcoords[1]);
	glVertex2f(m_width - m_theme.sbar.width, m_size);
	glTexCoord2f(texcoords[2], texcoords[1]);
	glVertex2f(m_width, m_size);
	glTexCoord2f(texcoords[0], texcoords[3]);
	glVertex2f(m_width - m_theme.sbar.width, m_size - m_theme.sbar.buttonHeight);
	glTexCoord2f(texcoords[2], texcoords[3]);
	glVertex2f(m_width, m_size - m_theme.sbar.buttonHeight);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
}

bool CUIScrollBar::LeftMouseButtonDown(int x, int y)
{
	if (!IsOnElement(x, y)) return false;
	if (y < m_theme.sbar.buttonHeight || y > m_size - m_theme.sbar.buttonHeight)
	{
		m_buttonPressed = true;
		return false;
	}
	m_mousePos = y;
	m_pressed = true;
	return true;
}

bool CUIScrollBar::LeftMouseButtonUp(int x, int y)
{
	if (!IsOnElement(x, y)) return false;
	int scrollSize = m_size * ((float)m_size / (float)m_contentSize);
	if (y < m_theme.sbar.buttonHeight && m_buttonPressed)//Up Button pressed
	{
		m_position -= (float)m_step * (m_size - 2 * m_theme.sbar.buttonHeight) / (float)m_contentSize;
	} 
	else if (y > m_size - m_theme.sbar.buttonHeight && m_buttonPressed)//Down Button pressed
	{
		m_position += (float)m_step * (m_size - 2 * m_theme.sbar.buttonHeight) / (float)m_contentSize;
	}
	else if (y < m_position || y > m_position + scrollSize)//Outside bar click
	{
		m_position = y;
	}
	else if(m_pressed) //Dragging
	{
		m_position += y - m_mousePos;
		m_pressed = false;
	}
	else
	{
		return false;
	}
	if(m_position < 0) m_position = 0;
	if(m_position > m_size - scrollSize) m_position = m_size - scrollSize;
	m_buttonPressed = false;
	return true;
}

bool CUIScrollBar::IsOnElement(int x, int y) const
{
	if (m_size >= m_contentSize) return false;
	return (x >= m_width - m_theme.sbar.width && x <= m_width && y >= 0.0f && y <= m_size);
}

int CUIScrollBar::GetPosition() const 
{ 
	return m_position * m_contentSize / (m_size - 2 * m_theme.sbar.buttonHeight); 
}