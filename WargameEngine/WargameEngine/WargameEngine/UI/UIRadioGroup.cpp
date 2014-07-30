#include "UIRadioGroup.h"
#include <GL/glut.h>
#include "../view/TextureManager.h"
#include "UIText.h"

void CUIRadioGroup::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(GetX(), GetY(), 0.0f);
	for(size_t i = 0; i < m_items.size(); ++i)
	{
		CTextureManager::GetInstance()->SetTexture(m_theme.texture);
		float y = m_theme.radiogroup.elementSize * i + (m_theme.radiogroup.elementSize - m_theme.radiogroup.buttonSize) / 2;
		glBegin(GL_TRIANGLE_STRIP);
		(i == m_selected)?glTexCoord2f(m_theme.radiogroup.selectedTexCoord[0], m_theme.radiogroup.selectedTexCoord[1]):glTexCoord2f(m_theme.radiogroup.texCoord[0], m_theme.radiogroup.texCoord[1]);
		glVertex2f(0.0f, y);
		(i == m_selected)?glTexCoord2f(m_theme.radiogroup.selectedTexCoord[0], m_theme.radiogroup.selectedTexCoord[3]):glTexCoord2f(m_theme.radiogroup.texCoord[0], m_theme.radiogroup.texCoord[3]);
		glVertex2f(0.0f, y + m_theme.radiogroup.buttonSize);
		(i == m_selected)?glTexCoord2f(m_theme.radiogroup.selectedTexCoord[2], m_theme.radiogroup.selectedTexCoord[1]):glTexCoord2f(m_theme.radiogroup.texCoord[2], m_theme.radiogroup.texCoord[1]);
		glVertex2f(m_theme.radiogroup.buttonSize, y);
		(i == m_selected)?glTexCoord2f(m_theme.radiogroup.selectedTexCoord[2], m_theme.radiogroup.selectedTexCoord[3]):glTexCoord2f(m_theme.radiogroup.texCoord[2], m_theme.radiogroup.texCoord[3]);
		glVertex2f(m_theme.radiogroup.buttonSize, y + m_theme.radiogroup.buttonSize);
		glEnd();
		PrintText(m_theme.radiogroup.buttonSize + 1, m_theme.radiogroup.elementSize * i, GetWidth(), m_theme.radiogroup.elementSize, m_items[i], m_theme.text);
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	CTextureManager::GetInstance()->SetTexture("");
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIRadioGroup::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		return true;
	}
	if(PointIsOnElement(x, y))
	{
		unsigned int index = (y - GetY()) / m_theme.radiogroup.elementSize;
		if(index >= 0 && index < m_items.size()) m_selected = index;
		if(m_onChange) m_onChange();
		SetFocus();
		return true;
	}
	return false;
}

void CUIRadioGroup::AddItem(std::string const& str)
{
	m_items.push_back(str);
	if(m_selected == -1)
	{
		m_selected = 0;
	}
}

std::string const CUIRadioGroup::GetText() const
{
	return m_items[m_selected];
}

void CUIRadioGroup::SetSelected(size_t index)
{
	m_selected = index;
}

void CUIRadioGroup::DeleteItem(size_t index)
{
	m_items.erase(m_items.begin() + index);
	if(m_selected == index) m_selected--;
	if(m_selected == -1 && !m_items.empty()) m_selected = 0;
}

void CUIRadioGroup::SetText(std::string const& text)
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