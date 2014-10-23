#include "UICheckBox.h"
#include "../view/gl.h"
#include "../view/TextureManager.h"
#include "UIText.h"

void CUICheckBox::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(GetX(), GetY(), 0.0f);
	CTextureManager::GetInstance()->SetTexture(m_theme.texture);
	glBegin(GL_QUADS);
		(m_state)?glTexCoord2f(m_theme.checkbox.checkedTexCoord[0], m_theme.checkbox.checkedTexCoord[1]):glTexCoord2f(m_theme.checkbox.texCoord[0], m_theme.checkbox.texCoord[1]);
		glVertex2f(0.0f, 0.0f);
		(m_state)?glTexCoord2f(m_theme.checkbox.checkedTexCoord[0], m_theme.checkbox.checkedTexCoord[3]):glTexCoord2f(m_theme.checkbox.texCoord[0], m_theme.checkbox.texCoord[3]);
		glVertex2f(0.0f, GetHeight() * m_theme.checkbox.checkboxSizeCoeff);
		(m_state)?glTexCoord2f(m_theme.checkbox.checkedTexCoord[2], m_theme.checkbox.checkedTexCoord[3]):glTexCoord2f(m_theme.checkbox.texCoord[2], m_theme.checkbox.texCoord[3]);
		glVertex2f(GetHeight() * m_theme.checkbox.checkboxSizeCoeff, GetHeight() * m_theme.checkbox.checkboxSizeCoeff);
		(m_state)?glTexCoord2f(m_theme.checkbox.checkedTexCoord[2], m_theme.checkbox.checkedTexCoord[1]):glTexCoord2f(m_theme.checkbox.texCoord[2], m_theme.checkbox.texCoord[1]);
		glVertex2f(GetHeight() * m_theme.checkbox.checkboxSizeCoeff, 0.0f);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	PrintText(GetHeight() * m_theme.checkbox.checkboxSizeCoeff + 1, 0, GetWidth(), GetHeight(), m_text, m_theme.text);
	CUIElement::Draw();
	glPopMatrix();
}

bool CUICheckBox::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		m_pressed = false;
		return true;
	}
	if(m_pressed && PointIsOnElement(x, y))
	{
		m_state = !m_state;
		m_pressed = false;
		return true;
	}
	m_pressed = false;
	return false;
}

bool CUICheckBox::LeftMouseButtonDown(int x, int y)
{
	if(!m_visible) return false;
	if (CUIElement::LeftMouseButtonDown(x, y))
	{
		return true;
	}
	if(PointIsOnElement(x, y))
	{
		m_pressed = true;
		return true;
	}
	return false;
}

void CUICheckBox::SetState(bool state)
{
	m_state = state;
}