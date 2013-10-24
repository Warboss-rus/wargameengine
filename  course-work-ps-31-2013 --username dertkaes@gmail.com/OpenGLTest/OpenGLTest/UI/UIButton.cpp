#include "UIButton.h"
#include <GL\glut.h>
#include "UIUtils.h"
#include "..\view\TextureManager.h"
#include "UIConfig.h"

void CUIButton::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	CTextureManager::GetInstance()->SetTexture(CUIConfig::texture);
	glBegin(GL_QUADS);
		(m_isPressed)?glTexCoord2d(CUIConfig::button.pressedTexCoord[0], CUIConfig::button.pressedTexCoord[1]):glTexCoord2d(CUIConfig::button.texCoord[0], CUIConfig::button.texCoord[1]);
		glVertex2d(0, 0);
		(m_isPressed)?glTexCoord2d(CUIConfig::button.pressedTexCoord[0], CUIConfig::button.pressedTexCoord[3]):glTexCoord2d(CUIConfig::button.texCoord[0], CUIConfig::button.texCoord[3]);
		glVertex2d(0, m_height);
		(m_isPressed)?glTexCoord2d(CUIConfig::button.pressedTexCoord[2], CUIConfig::button.pressedTexCoord[3]):glTexCoord2d(CUIConfig::button.texCoord[2], CUIConfig::button.texCoord[3]);
		glVertex2d(m_width, m_height);
		(m_isPressed)?glTexCoord2d(CUIConfig::button.pressedTexCoord[2], CUIConfig::button.pressedTexCoord[1]):glTexCoord2d(CUIConfig::button.texCoord[2], CUIConfig::button.texCoord[1]);
		glVertex2d(m_width, 0);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	glColor3f(CUIConfig::textColor[0], CUIConfig::textColor[1], CUIConfig::textColor[2]);
	int fontx = (m_width - glutBitmapLength(CUIConfig::font, (unsigned char*)m_text.c_str())) / 2 ;
	int fonty = (m_height + CUIConfig::fontHeight) / 2;
	PrintText(fontx, fonty, m_text.c_str(), CUIConfig::font);
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIButton::LeftMouseButtonUp(int x, int y)
{
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		m_isPressed = false;
		return true;
	}
	if(m_isPressed && PointIsOnElement(x, y))
	{
		m_onClick();
		m_isPressed = false;
		return true;
	}
	m_isPressed = false;
	return false;
}

bool CUIButton::LeftMouseButtonDown(int x, int y)
{
	if (CUIElement::LeftMouseButtonDown(x, y))
	{
		return true;
	}
	if(PointIsOnElement(x, y))
	{
		m_isPressed = true;
		return true;
	}
	return false;
}