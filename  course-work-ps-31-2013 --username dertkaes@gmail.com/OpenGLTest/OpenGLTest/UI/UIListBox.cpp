#include "UIListBox.h"
#include "UIText.h"
#include <GL\glut.h>
#include "..\view\TextureManager.h"

void CUIListBox::Draw() const
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
		glVertex2d(m_theme.listbox.borderSize, m_theme.listbox.borderSize);
		glVertex2d(m_theme.listbox.borderSize, m_height - m_theme.listbox.borderSize);
		glVertex2d(m_width - m_theme.listbox.borderSize, m_height - m_theme.listbox.borderSize);
		glVertex2d(m_width - m_theme.listbox.borderSize, m_theme.listbox.borderSize);
	glEnd();
	glColor3f(m_theme.text.color[0], m_theme.text.color[1], m_theme.text.color[2]);
	int fonty = (m_height + m_theme.text.fontHeight) / 2;
	if(m_selected >= 0)	PrintText(m_theme.listbox.borderSize, fonty, m_items[m_selected].c_str(), m_theme.text.font);
	glColor3f(0.6f,0.6f,0.6f);
	CTextureManager::GetInstance()->SetTexture(m_theme.texture);
	glBegin(GL_QUADS);
		(m_expanded)?glTexCoord2d(m_theme.listbox.expandedTexCoord[0], m_theme.listbox.expandedTexCoord[1]):glTexCoord2d(m_theme.listbox.texCoord[0], m_theme.listbox.texCoord[1]);
		glVertex2d(m_width - m_height * m_theme.listbox.buttonWidthCoeff, 0);
		(m_expanded)?glTexCoord2d(m_theme.listbox.expandedTexCoord[0], m_theme.listbox.expandedTexCoord[3]):glTexCoord2d(m_theme.listbox.texCoord[0], m_theme.listbox.texCoord[3]);
		glVertex2d(m_width - m_height * m_theme.listbox.buttonWidthCoeff, m_height);
		(m_expanded)?glTexCoord2d(m_theme.listbox.expandedTexCoord[2], m_theme.listbox.expandedTexCoord[3]):glTexCoord2d(m_theme.listbox.texCoord[2], m_theme.listbox.texCoord[3]);
		glVertex2d(m_width, m_height);
		(m_expanded)?glTexCoord2d(m_theme.listbox.expandedTexCoord[2], m_theme.listbox.expandedTexCoord[1]):glTexCoord2d(m_theme.listbox.texCoord[2], m_theme.listbox.texCoord[1]);
		glVertex2d(m_width, 0);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	if(m_expanded)
	{
		glColor3f(m_theme.textfieldColor[0], m_theme.textfieldColor[1], m_theme.textfieldColor[2]);
		glBegin(GL_QUADS);
			glVertex2d(0, m_height);
			glVertex2d(0, m_height * (m_items.size() + 1));
			glVertex2d(m_width, m_height * (m_items.size() + 1));
			glVertex2d(m_width, m_height);
		glEnd();
		glColor3f(m_theme.text.color[0], m_theme.text.color[1], m_theme.text.color[2]);
		for(size_t i = 0; i < m_items.size(); ++i)
		{
			PrintText(m_theme.listbox.borderSize, m_height * (i + 1) + fonty, m_items[i].c_str(), m_theme.text.font);
		}
	}
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIListBox::LeftMouseButtonDown(int x, int y)
{
	if(CUIElement::LeftMouseButtonUp(x, y))
		return true;
	if(PointIsOnElement(x, y))
	{
		m_pressed = true;
		return true;
	}
	return false;
}

bool CUIListBox::LeftMouseButtonUp(int x, int y)
{
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		m_pressed = false;
		return true;
	}
	if(PointIsOnElement(x, y))
	{
		if(m_pressed)
		{
			if(m_expanded && PointIsOnElement(x, y))
			{
				int index = (y - m_y) / m_height;
				if(index > 0) m_selected = index - 1;
			}
			m_expanded = !m_expanded;
		}
		SetFocus();
		m_pressed = false;
		return true;
	}
	else
	{
		m_expanded = false;
	}
	m_pressed = false;
	return false;
}

void CUIListBox::AddItem(std::string const& str)
{
	m_items.push_back(str);
	if(m_selected == -1)
	{
		m_selected = 0;
	}
}

std::string const& CUIListBox::GetText() const
{
	return m_items[m_selected];
}

void CUIListBox::SetSelected(size_t index)
{
	m_selected = index;
}

int CUIListBox::GetHeight() const
{
	if(m_expanded && !m_items.empty())
	{
		return m_height * (m_items.size() + 1);
	}
	return m_height;
}

void CUIListBox::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if(m_selected == index) m_selected--;
	if(m_selected == -1 && !m_items.empty()) m_selected = 0;
}

void CUIListBox::SetText(std::string const& text)
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