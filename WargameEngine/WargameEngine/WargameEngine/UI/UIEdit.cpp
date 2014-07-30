#include "UIEdit.h"
#include "UIText.h"
#include <GL/glut.h>
#include "../view/TextureManager.h"

void CUIEdit::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(GetX(), GetY(), 0.0f);
	glColor3f(m_theme.defaultColor[0], m_theme.defaultColor[1], m_theme.defaultColor[2]);
	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, GetHeight());
		glVertex2i(GetWidth(), GetHeight());
		glVertex2i(GetWidth(), 0);
	glEnd();
	glColor3f(m_theme.textfieldColor[0], m_theme.textfieldColor[1], m_theme.textfieldColor[2]);
	glBegin(GL_QUADS);
		glVertex2i(m_theme.edit.borderSize, m_theme.edit.borderSize);
		glVertex2i(m_theme.edit.borderSize, m_height - m_theme.edit.borderSize);
		glVertex2i(GetWidth() - m_theme.edit.borderSize, GetHeight() - m_theme.edit.borderSize);
		glVertex2i(GetWidth() - m_theme.edit.borderSize, m_theme.edit.borderSize);
	glEnd();
	int fonty = (GetHeight() + m_theme.edit.text.fontSize) / 2;
	if(IsFocused(NULL))
	{
		int cursorpos = m_theme.edit.borderSize + GetStringWidth(m_theme.edit.text, m_text);
		glColor3ub(0, 0, 0);
		glBegin(GL_LINES);
		glVertex2i(cursorpos, fonty);
		glVertex2i(cursorpos, fonty - m_theme.edit.text.fontSize);
		glEnd();
	}
	if(m_pos != m_beginSelection)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		int fontwidth = GetStringWidth(m_theme.edit.text, m_text);
		glBegin(GL_QUADS);
		glVertex2i(m_theme.edit.borderSize + m_beginSelection * fontwidth, fonty);
		glVertex2i(m_theme.edit.borderSize + m_beginSelection * fontwidth, fonty - GetStringHeight(m_theme.edit.text, m_text));
		glVertex2i(m_theme.edit.borderSize + m_pos * fontwidth, fonty - GetStringHeight(m_theme.edit.text, m_text));
		glVertex2i(m_theme.edit.borderSize + m_pos * fontwidth, fonty);
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
		size_t pos = (x - GetX()) / GetStringHeight(m_theme.edit.text, "0");
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
		size_t pos = (x - GetX()) / GetStringHeight(m_theme.edit.text, "0");
		m_beginSelection = (pos > m_text.size())?m_text.size():pos;
		m_pos = m_beginSelection;
		SetFocus();
		return true;
	}
	return false;
}