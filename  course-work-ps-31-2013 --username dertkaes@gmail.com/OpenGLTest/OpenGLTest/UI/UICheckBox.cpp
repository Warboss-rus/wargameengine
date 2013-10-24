#include "UICheckBox.h"
#include <GL\glut.h>
#include "UIUtils.h"
#include "..\view\TextureManager.h"
#include "UIConfig.h"

void CUICheckBox::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	CTextureManager::GetInstance()->SetTexture(CUIConfig::texture);
	glBegin(GL_QUADS);
		(m_state)?glTexCoord2d(CUIConfig::checkbox.checkedTexCoord[0], CUIConfig::checkbox.checkedTexCoord[1]):glTexCoord2d(CUIConfig::checkbox.texCoord[0], CUIConfig::checkbox.texCoord[1]);
		glVertex2d(0, 0);
		(m_state)?glTexCoord2d(CUIConfig::checkbox.checkedTexCoord[0], CUIConfig::checkbox.checkedTexCoord[3]):glTexCoord2d(CUIConfig::checkbox.texCoord[0], CUIConfig::checkbox.texCoord[3]);
		glVertex2d(0, m_height * CUIConfig::checkbox.checkboxSizeCoeff);
		(m_state)?glTexCoord2d(CUIConfig::checkbox.checkedTexCoord[2], CUIConfig::checkbox.checkedTexCoord[3]):glTexCoord2d(CUIConfig::checkbox.texCoord[2], CUIConfig::checkbox.texCoord[3]);
		glVertex2d(m_height * CUIConfig::checkbox.checkboxSizeCoeff, m_height * CUIConfig::checkbox.checkboxSizeCoeff);
		(m_state)?glTexCoord2d(CUIConfig::checkbox.checkedTexCoord[2], CUIConfig::checkbox.checkedTexCoord[1]):glTexCoord2d(CUIConfig::checkbox.texCoord[2], CUIConfig::checkbox.texCoord[1]);
		glVertex2d(m_height * CUIConfig::checkbox.checkboxSizeCoeff, 0);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	glColor3f(CUIConfig::textColor[0], CUIConfig::textColor[1], CUIConfig::textColor[2]);
	int fontx = m_height * CUIConfig::checkbox.checkboxSizeCoeff + (m_width - m_height * CUIConfig::checkbox.checkboxSizeCoeff - glutBitmapLength(CUIConfig::font, (unsigned char*)m_text.c_str())) / 2 ;
	int fonty = (m_height + CUIConfig::fontHeight) / 2;
	PrintText(fontx, fonty, m_text.c_str(), CUIConfig::font);
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