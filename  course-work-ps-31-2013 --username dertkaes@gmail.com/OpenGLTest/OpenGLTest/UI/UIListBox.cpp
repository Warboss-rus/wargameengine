#include "UIListBox.h"
#include "UIUtils.h"
#include <GL\glut.h>
#include "..\view\TextureManager.h"

void CUIListBox::Draw() const
{
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	glColor3f(0.6f,0.6f,0.6f);
	glBegin(GL_QUADS);
		glVertex2d(0, 0);
		glVertex2d(0, m_height);
		glVertex2d(m_width, m_height);
		glVertex2d(m_width, 0);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2d(2, 2);
		glVertex2d(2, m_height - 2);
		glVertex2d(m_width - 2, m_height - 2);
		glVertex2d(m_width - 2, 2);
	glEnd();
	glColor3f(0.0f,0.0f,0.0f);
	int fonty = (m_height + 20) / 2;
	if(m_selected >= 0)	PrintText(2, fonty, m_items[m_selected].c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
	glColor3f(0.6f,0.6f,0.6f);
	CTextureManager::GetInstance()->SetTexture("g2Default.png");
	glBegin(GL_QUADS);
		glTexCoord2d(0.27734, 0.80469);
		glVertex2d(m_width - m_height * 2 / 3, 0);
		glTexCoord2d(0.27734, 0.74219);
		glVertex2d(m_width - m_height * 2 / 3, m_height);
		glTexCoord2d(0.3125, 0.74219);
		glVertex2d(m_width, m_height);
		glTexCoord2d(0.3125, 0.80469);
		glVertex2d(m_width, 0);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	if(m_expanded)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex2d(0, m_height);
			glVertex2d(0, m_height * (m_items.size() + 1));
			glVertex2d(m_width, m_height * (m_items.size() + 1));
			glVertex2d(m_width, m_height);
		glEnd();
		glColor3f(0.0f,0.0f,0.0f);
		for(size_t i = 0; i < m_items.size(); ++i)
		{
			PrintText(2, m_height * (i + 1) + fonty, m_items[i].c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
		}
	glEnd();
	}
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIListBox::LeftMouseButtonUp(int x, int y)
{
	if(!CUIElement::LeftMouseButtonDown(x, y))
	{
		if(m_expanded)
		{
			int index = (y - m_y) / m_height;
			if(index > 0) m_selected = index - 1;
		}
		m_expanded = !m_expanded;
	}
	return true;
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