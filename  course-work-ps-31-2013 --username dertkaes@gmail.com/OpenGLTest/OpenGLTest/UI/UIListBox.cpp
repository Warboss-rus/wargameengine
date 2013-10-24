#include "UIListBox.h"
#include "UIUtils.h"
#include <GL\glut.h>
#include "..\view\TextureManager.h"
#include "UIConfig.h"

void CUIListBox::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	glColor3f(CUIConfig::defaultColor[0], CUIConfig::defaultColor[1], CUIConfig::defaultColor[2]);
	glBegin(GL_QUADS);
		glVertex2d(0, 0);
		glVertex2d(0, m_height);
		glVertex2d(m_width, m_height);
		glVertex2d(m_width, 0);
	glEnd();
	glColor3f(CUIConfig::textfieldColor[0], CUIConfig::textfieldColor[1], CUIConfig::textfieldColor[2]);
	glBegin(GL_QUADS);
		glVertex2d(CUIConfig::listbox.borderSize, CUIConfig::listbox.borderSize);
		glVertex2d(CUIConfig::listbox.borderSize, m_height - CUIConfig::listbox.borderSize);
		glVertex2d(m_width - CUIConfig::listbox.borderSize, m_height - CUIConfig::listbox.borderSize);
		glVertex2d(m_width - CUIConfig::listbox.borderSize, CUIConfig::listbox.borderSize);
	glEnd();
	glColor3f(CUIConfig::textColor[0], CUIConfig::textColor[1], CUIConfig::textColor[2]);
	int fonty = (m_height + CUIConfig::fontHeight) / 2;
	if(m_selected >= 0)	PrintText(CUIConfig::listbox.borderSize, fonty, m_items[m_selected].c_str(), CUIConfig::font);
	glColor3f(0.6f,0.6f,0.6f);
	CTextureManager::GetInstance()->SetTexture(CUIConfig::texture);
	glBegin(GL_QUADS);
		(m_expanded)?glTexCoord2d(CUIConfig::listbox.expandedTexCoord[0], CUIConfig::listbox.expandedTexCoord[1]):glTexCoord2d(CUIConfig::listbox.texCoord[0], CUIConfig::listbox.texCoord[1]);
		glVertex2d(m_width - m_height * CUIConfig::listbox.buttonWidthCoeff, 0);
		(m_expanded)?glTexCoord2d(CUIConfig::listbox.expandedTexCoord[0], CUIConfig::listbox.expandedTexCoord[3]):glTexCoord2d(CUIConfig::listbox.texCoord[0], CUIConfig::listbox.texCoord[3]);
		glVertex2d(m_width - m_height * CUIConfig::listbox.buttonWidthCoeff, m_height);
		(m_expanded)?glTexCoord2d(CUIConfig::listbox.expandedTexCoord[2], CUIConfig::listbox.expandedTexCoord[3]):glTexCoord2d(CUIConfig::listbox.texCoord[2], CUIConfig::listbox.texCoord[3]);
		glVertex2d(m_width, m_height);
		(m_expanded)?glTexCoord2d(CUIConfig::listbox.expandedTexCoord[2], CUIConfig::listbox.expandedTexCoord[1]):glTexCoord2d(CUIConfig::listbox.texCoord[2], CUIConfig::listbox.texCoord[1]);
		glVertex2d(m_width, 0);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	if(m_expanded)
	{
		glColor3f(CUIConfig::textfieldColor[0], CUIConfig::textfieldColor[1], CUIConfig::textfieldColor[2]);
		glBegin(GL_QUADS);
			glVertex2d(0, m_height);
			glVertex2d(0, m_height * (m_items.size() + 1));
			glVertex2d(m_width, m_height * (m_items.size() + 1));
			glVertex2d(m_width, m_height);
		glEnd();
		glColor3f(CUIConfig::textColor[0], CUIConfig::textColor[1], CUIConfig::textColor[2]);
		for(size_t i = 0; i < m_items.size(); ++i)
		{
			PrintText(CUIConfig::listbox.borderSize, m_height * (i + 1) + fonty, m_items[i].c_str(), CUIConfig::font);
		}
	glEnd();
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

std::string CUIListBox::GetSelectedItem() const
{
	return m_items[m_selected];
}

void CUIListBox::SetSelected(int index)
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