#include "UIList.h"
#include "UIText.h"
#include <GL\glut.h>
#include "..\view\TextureManager.h"

void CUIList::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(GetX(), GetY(), 0);
	glColor3f(m_theme.defaultColor[0], m_theme.defaultColor[1], m_theme.defaultColor[2]);
	glBegin(GL_QUADS);
		glVertex2d(0, 0);
		glVertex2d(0, GetHeight());
		glVertex2d(GetWidth(), GetHeight());
		glVertex2d(GetWidth(), 0);
	glEnd();
	glColor3f(m_theme.textfieldColor[0], m_theme.textfieldColor[1], m_theme.textfieldColor[2]);
	glBegin(GL_QUADS);
		glVertex2d(m_theme.listbox.borderSize, m_theme.listbox.borderSize);
		glVertex2d(m_theme.listbox.borderSize, GetHeight() - m_theme.listbox.borderSize);
		glVertex2d(GetWidth() - m_theme.listbox.borderSize, GetHeight() - m_theme.listbox.borderSize);
		glVertex2d(GetWidth() - m_theme.listbox.borderSize, m_theme.listbox.borderSize);
	glEnd();
	if(m_selected > -1)
	{
		glColor3f(0.2f, 0.2f, 1.0f);
		glBegin(GL_QUADS);
			glVertex2d(m_theme.listbox.borderSize, m_theme.listbox.borderSize + m_theme.text.fontHeight * m_selected);
			glVertex2d(m_theme.listbox.borderSize, m_theme.listbox.borderSize + m_theme.text.fontHeight * (m_selected + 1));
			glVertex2d(GetWidth() - m_theme.listbox.borderSize, m_theme.listbox.borderSize + m_theme.text.fontHeight * (m_selected + 1));
			glVertex2d(GetWidth() - m_theme.listbox.borderSize, m_theme.listbox.borderSize + m_theme.text.fontHeight * m_selected);
		glEnd();
	}
	glColor3f(m_theme.text.color[0], m_theme.text.color[1], m_theme.text.color[2]);
	for(size_t i = 0; i < m_items.size(); ++i)
	{
		PrintText(m_theme.listbox.borderSize, m_theme.listbox.borderSize + m_theme.text.fontHeight * i, GetWidth(), 20, m_items[i], m_theme.text);
	}
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIList::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		return true;
	}
	if(PointIsOnElement(x, y))
	{
		int index = (y - GetY()) / 20;
		if(index >= 0 && index < m_items.size()) m_selected = index;
		SetFocus();
		return true;
	}
	return false;
}

void CUIList::AddItem(std::string const& str)
{
	m_items.push_back(str);
	if(m_selected == -1)
	{
		m_selected = 0;
	}
}

std::string const CUIList::GetText() const
{
	return m_items[m_selected];
}

void CUIList::SetSelected(size_t index)
{
	m_selected = index;
}

void CUIList::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if(m_selected == index) m_selected--;
	if(m_selected == -1 && !m_items.empty()) m_selected = 0;
}

void CUIList::SetText(std::string const& text)
{
	for(size_t i = 0; i < m_items.size(); ++i)
	{
		if(m_items[i] == text)
		{
			m_selected = i;
			return;
		}
	}
}