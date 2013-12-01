#include "UIEdit.h"
#include "UIText.h"
#include <GL\glut.h>
#include "..\view\TextureManager.h"
#include "UIText.h"

void CUIEdit::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	glColor3f(m_theme.defaultColor[0], m_theme.defaultColor[1], m_theme.defaultColor[2]);
	glBegin(GL_QUADS);
		glVertex2d(0, 0);
		glVertex2d(0, m_height);
		glVertex2d(m_width, m_height);
		glVertex2d(m_width, 0);
	glEnd();
	glColor3f(m_theme.textfieldColor[0], m_theme.textfieldColor[1], m_theme.textfieldColor[2]);
	glBegin(GL_QUADS);
		glVertex2d(m_theme.edit.borderSize, m_theme.edit.borderSize);
		glVertex2d(m_theme.edit.borderSize, m_height - m_theme.edit.borderSize);
		glVertex2d(m_width - m_theme.edit.borderSize, m_height - m_theme.edit.borderSize);
		glVertex2d(m_width - m_theme.edit.borderSize, m_theme.edit.borderSize);
	glEnd();
	int fonty = (m_height + m_theme.text.fontHeight) / 2;
	if(IsFocused(NULL))
	{
		double cursorpos = m_theme.edit.borderSize + m_pos * glutBitmapLength(m_theme.text.font, (const unsigned char*)"0");
		glColor3b(0, 0, 0);
		glBegin(GL_LINES);
		glVertex2d(cursorpos, fonty);
		glVertex2d(cursorpos, fonty - m_theme.text.fontHeight);
		glEnd();
	}
	if(m_pos != m_beginSelection)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		double fontwidth = glutBitmapLength(m_theme.text.font, (const unsigned char*)"0");
		glBegin(GL_QUADS);
		glVertex2d(m_theme.edit.borderSize + m_beginSelection * fontwidth, fonty);
		glVertex2d(m_theme.edit.borderSize + m_beginSelection * fontwidth, fonty - m_theme.text.fontHeight);
		glVertex2d(m_theme.edit.borderSize + m_pos * fontwidth, fonty - m_theme.text.fontHeight);
		glVertex2d(m_theme.edit.borderSize + m_pos * fontwidth, fonty);
		glEnd();
	}
	PrintText(m_theme.edit.borderSize, m_theme.edit.borderSize, m_width - 2 * m_theme.edit.borderSize, m_height - 2 *m_theme.edit.borderSize, m_text, m_theme.text);
	CTextureManager::GetInstance()->SetTexture("");
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIEdit::OnKeyPress(unsigned char key)
{
	if(!m_visible) return false;
	if(CUIElement::OnKeyPress(key)) 
		return true;
	if(!IsFocused(NULL))
	{
		return false;
	}
	if(key < 32 && key != 8) 
		return false;
	if(key != 127 && key != 8)
	{
		char str[2];
		str[0] = key;
		str[1] = '\0';
		m_text.insert(m_pos, str);
		m_pos++;
		m_beginSelection++;
	}
	if(key == 8 && m_pos > 0)
	{
		m_text.erase(m_pos - 1, 1);
		m_pos--;
		m_beginSelection--;
	}
	if(key == 127)
	{
		if(m_pos != m_beginSelection)
		{
			size_t begin = (m_pos < m_beginSelection)?m_pos:m_beginSelection;
			size_t count = (m_pos - m_beginSelection > 0)?m_pos - m_beginSelection:m_beginSelection - m_pos;
			m_text.erase(begin, count);
			m_pos = begin;
			m_beginSelection = begin;
		}
		else
		{
			if(m_pos < m_text.size())
			{
				m_text.erase(m_pos, 1);
			}
		}
	}
	return true;
}

bool CUIEdit::OnSpecialKeyPress(int key)
{
	if(!m_visible) return false;
	if(CUIElement::OnSpecialKeyPress(key)) 
		return true;
	if(!IsFocused(NULL))
	{
		return false;
	}
	switch (key) 
	{
	case GLUT_KEY_LEFT:
		{
			if(m_pos > 0) m_pos--;
			if(m_beginSelection > 0) m_beginSelection--;
			return true;
		}break;
	case GLUT_KEY_RIGHT:
		{
			if(m_pos < m_text.size()) m_pos++;
			if(m_beginSelection < m_text.size()) m_beginSelection++;
			return true;
		}break;
	case GLUT_KEY_HOME:
		{
			m_pos = 0;
			return true;
		}break;
	case GLUT_KEY_END:
		{
			m_pos = m_text.size();
			return true;
		}break;
	}
	return false;
}

bool CUIEdit::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
		return true;
	if(PointIsOnElement(x, y))
	{
		size_t pos = (x - m_x) / glutBitmapLength(m_theme.text.font, (const unsigned char*)"0");
		m_pos = (pos > m_text.size())?m_text.size():pos;
		SetFocus();
		return true;
	}
	else
	{
		m_beginSelection = m_pos;
	}
	return false;
}

bool CUIEdit::LeftMouseButtonDown(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonDown(x, y))
		return true;
	if(PointIsOnElement(x, y))
	{
		size_t pos = (x - m_x) / glutBitmapLength(m_theme.text.font, (const unsigned char*)"0");
		m_beginSelection = (pos > m_text.size())?m_text.size():pos;
		m_pos = m_beginSelection;
		SetFocus();
		return true;
	}
	return false;
}