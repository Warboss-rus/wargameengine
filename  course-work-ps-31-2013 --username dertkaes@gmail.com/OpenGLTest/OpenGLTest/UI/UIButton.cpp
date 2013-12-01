#include "UIButton.h"
#include <GL\glut.h>
#include "..\view\TextureManager.h"
#include "UIText.h"

void CUIButton::Draw() const
{
	if(!m_visible)
		return;
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	CTextureManager::GetInstance()->SetTexture(m_theme.texture);
	glBegin(GL_QUADS);
		(m_isPressed)?glTexCoord2d(m_theme.button.pressedTexCoord[0], m_theme.button.pressedTexCoord[1]):glTexCoord2d(m_theme.button.texCoord[0], m_theme.button.texCoord[1]);
		glVertex2d(0, 0);
		(m_isPressed)?glTexCoord2d(m_theme.button.pressedTexCoord[0], m_theme.button.pressedTexCoord[3]):glTexCoord2d(m_theme.button.texCoord[0], m_theme.button.texCoord[3]);
		glVertex2d(0, m_height);
		(m_isPressed)?glTexCoord2d(m_theme.button.pressedTexCoord[2], m_theme.button.pressedTexCoord[3]):glTexCoord2d(m_theme.button.texCoord[2], m_theme.button.texCoord[3]);
		glVertex2d(m_width, m_height);
		(m_isPressed)?glTexCoord2d(m_theme.button.pressedTexCoord[2], m_theme.button.pressedTexCoord[1]):glTexCoord2d(m_theme.button.texCoord[2], m_theme.button.texCoord[1]);
		glVertex2d(m_width, 0);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	CUITheme::sText text = m_theme.text;
	text.aligment = text.center;
	PrintText(0, 0, m_width, m_height, m_text, text);
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIButton::LeftMouseButtonUp(int x, int y)
{
	if(!m_visible) return false;
	if(CUIElement::LeftMouseButtonUp(x, y))
	{
		m_isPressed = false;
		return true;
	}
	if(m_isPressed && PointIsOnElement(x, y))
	{
		m_onClick();
		SetFocus();
		m_isPressed = false;
		return true;
	}
	m_isPressed = false;
	return false;
}

bool CUIButton::LeftMouseButtonDown(int x, int y)
{
	if(!m_visible) return false;
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