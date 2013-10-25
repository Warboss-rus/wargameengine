#include "UICheckBox.h"
#include <GL\glut.h>
#include "..\view\TextureManager.h"

void CUICheckBox::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	CTextureManager::GetInstance()->SetTexture(m_theme.texture);
	glBegin(GL_QUADS);
		(m_state)?glTexCoord2d(m_theme.checkbox.checkedTexCoord[0], m_theme.checkbox.checkedTexCoord[1]):glTexCoord2d(m_theme.checkbox.texCoord[0], m_theme.checkbox.texCoord[1]);
		glVertex2d(0, 0);
		(m_state)?glTexCoord2d(m_theme.checkbox.checkedTexCoord[0], m_theme.checkbox.checkedTexCoord[3]):glTexCoord2d(m_theme.checkbox.texCoord[0], m_theme.checkbox.texCoord[3]);
		glVertex2d(0, m_height * m_theme.checkbox.checkboxSizeCoeff);
		(m_state)?glTexCoord2d(m_theme.checkbox.checkedTexCoord[2], m_theme.checkbox.checkedTexCoord[3]):glTexCoord2d(m_theme.checkbox.texCoord[2], m_theme.checkbox.texCoord[3]);
		glVertex2d(m_height * m_theme.checkbox.checkboxSizeCoeff, m_height * m_theme.checkbox.checkboxSizeCoeff);
		(m_state)?glTexCoord2d(m_theme.checkbox.checkedTexCoord[2], m_theme.checkbox.checkedTexCoord[1]):glTexCoord2d(m_theme.checkbox.texCoord[2], m_theme.checkbox.texCoord[1]);
		glVertex2d(m_height * m_theme.checkbox.checkboxSizeCoeff, 0);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	m_text.Draw();
	CUIElement::Draw();
	glPopMatrix();
}

bool CUICheckBox::LeftMouseButtonUp(int x, int y)
{
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