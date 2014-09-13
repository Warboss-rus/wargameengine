#include "UIComboBox.h"
#include "UIText.h"
#include "../gl.h"
#include "../view/TextureManager.h"

void CUIComboBox::Draw() const
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
		glVertex2i(m_theme.combobox.borderSize, m_theme.combobox.borderSize);
		glVertex2i(m_theme.combobox.borderSize, GetHeight() - m_theme.combobox.borderSize);
		glVertex2i(GetWidth() - m_theme.combobox.borderSize, GetHeight() - m_theme.combobox.borderSize);
		glVertex2i(GetWidth() - m_theme.combobox.borderSize, m_theme.combobox.borderSize);
	glEnd();
	glColor3f(m_theme.text.color[0], m_theme.text.color[1], m_theme.text.color[2]);
	if(m_selected >= 0)	PrintText(m_theme.combobox.borderSize, m_theme.combobox.borderSize, GetWidth(), GetHeight(), m_items[m_selected], m_theme.combobox.text);
	glColor3f(0.6f,0.6f,0.6f);
	CTextureManager::GetInstance()->SetTexture(m_theme.texture);
	glBegin(GL_QUADS);
		(m_expanded)?glTexCoord2f(m_theme.combobox.expandedTexCoord[0], m_theme.combobox.expandedTexCoord[1]):glTexCoord2f(m_theme.combobox.texCoord[0], m_theme.combobox.texCoord[1]);
		glVertex2f(GetWidth() - GetHeight() * m_theme.combobox.buttonWidthCoeff, 0.0f);
		(m_expanded)?glTexCoord2f(m_theme.combobox.expandedTexCoord[0], m_theme.combobox.expandedTexCoord[3]):glTexCoord2f(m_theme.combobox.texCoord[0], m_theme.combobox.texCoord[3]);
		glVertex2f(GetWidth() - GetHeight() * m_theme.combobox.buttonWidthCoeff, GetHeight());
		(m_expanded)?glTexCoord2f(m_theme.combobox.expandedTexCoord[2], m_theme.combobox.expandedTexCoord[3]):glTexCoord2f(m_theme.combobox.texCoord[2], m_theme.combobox.texCoord[3]);
		glVertex2i(GetWidth(), GetHeight());
		(m_expanded)?glTexCoord2f(m_theme.combobox.expandedTexCoord[2], m_theme.combobox.expandedTexCoord[1]):glTexCoord2f(m_theme.combobox.texCoord[2], m_theme.combobox.texCoord[1]);
		glVertex2i(GetWidth(), 0);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	if(m_expanded)
	{
		glColor3f(m_theme.textfieldColor[0], m_theme.textfieldColor[1], m_theme.textfieldColor[2]);
		glBegin(GL_QUADS);
			glVertex2i(0, GetHeight());
			glVertex2i(0, GetHeight() + m_theme.combobox.elementSize * m_items.size());
			glVertex2i(GetWidth(), GetHeight() + m_theme.combobox.elementSize * m_items.size());
			glVertex2i(GetWidth(), GetHeight());
		glEnd();
		glColor3f(m_theme.text.color[0], m_theme.text.color[1], m_theme.text.color[2]);
		for (size_t i = m_scrollbar.GetPosition() / m_theme.combobox.elementSize; i < m_items.size(); ++i)
		{
			//if (GetHeight() + m_theme.list.elementSize * (i - m_scrollbar.GetPosition() / m_theme.list.elementSize) > GetHeight()) break;
			PrintText(m_theme.combobox.borderSize, GetHeight() + m_theme.combobox.elementSize * (i - m_scrollbar.GetPosition() / m_theme.combobox.elementSize), GetWidth(), m_theme.combobox.elementSize, m_items[i], m_theme.combobox.text);
		}
		glPushMatrix();
		glTranslatef(0.0f, GetHeight(), 0.0f);
		m_scrollbar.Draw();
		glPopMatrix();
	}
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIComboBox::LeftMouseButtonDown(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonDown(x, y))
		return true;
	if(PointIsOnElement(x, y))
	{
		if (m_expanded)
		{
			if (m_scrollbar.LeftMouseButtonDown(x - GetX(), y - GetY() - GetHeight())) return true;
		}
		m_pressed = true;
		return true;

	}
	return false;
}

bool CUIComboBox::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		m_pressed = false;
		return true;
	}
	if (m_expanded && m_scrollbar.LeftMouseButtonUp(x - GetX(), y - GetY() - GetHeight())) return true;
	if(PointIsOnElement(x, y))
	{
		if(m_pressed)
		{
			if(m_expanded && PointIsOnElement(x, y))
			{
				int index = (y - GetHeight()) / m_theme.combobox.elementSize;
				if(index > 0) m_selected = index - 1;
				if(m_onChange) m_onChange();
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

void CUIComboBox::AddItem(std::string const& str)
{
	m_items.push_back(str);
	if(m_selected == -1)
	{
		m_selected = 0;
	}
	m_scrollbar.Update(m_windowHeight - GetX() - GetHeight(), m_theme.combobox.elementSize * (m_items.size() + 1), GetWidth(), m_theme.combobox.elementSize);
}

std::string const CUIComboBox::GetText() const
{
	return m_items[m_selected];
}

void CUIComboBox::SetSelected(size_t index)
{
	m_selected = index;
}

bool CUIComboBox::PointIsOnElement(int x, int y) const
{
	int height = GetHeight();
	if(m_expanded && !m_items.empty())
	{
		height *= m_items.size() + 1;
	}
	if(x > GetX() && x < GetX() + GetWidth()	&& y > GetY() && y < GetY() + height)
		return true;
	return false;
}

void CUIComboBox::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if(m_selected == index) m_selected--;
	if(m_selected == -1 && !m_items.empty()) m_selected = 0;
	m_scrollbar.Update(m_windowHeight - GetX() - GetHeight(), m_theme.combobox.elementSize * (m_items.size() + 1), GetWidth(), m_theme.combobox.elementSize);
}

void CUIComboBox::SetText(std::string const& text)
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

void CUIComboBox::Resize(int windowHeight, int windowWidth) 
{
	CUIElement::Resize(windowHeight, windowWidth);
	m_scrollbar.Update(m_windowHeight - GetX() - GetHeight(), m_theme.combobox.elementSize * (m_items.size() + 1), GetWidth(), m_theme.combobox.elementSize);
}