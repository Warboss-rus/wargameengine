#include "UIButton.h"
#include <GL\glut.h>
#include "UIUtils.h"
#include "..\view\TextureManager.h"

void CUIButton::Draw() const
{
	glPushMatrix();
	glTranslatef(m_x, m_y, 0);
	glColor3f(0.6f,0.6f,0.6f);
	CTextureManager::GetInstance()->SetTexture("g2Default.png");
	glBegin(GL_QUADS);
		(m_isPressed)?glTexCoord2d(0.02344, 0.66016):glTexCoord2d(0.02344, 0.59375);
		glVertex2d(0, 0);
		(m_isPressed)?glTexCoord2d(0.02344, 0.60157):glTexCoord2d(0.02344, 0.53516);
		glVertex2d(0, m_height);
		(m_isPressed)?glTexCoord2d(0.22266, 0.60157):glTexCoord2d(0.22266, 0.53516);
		glVertex2d(m_width, m_height);
		(m_isPressed)?glTexCoord2d(0.22266, 0.66016):glTexCoord2d(0.22266, 0.59375);
		glVertex2d(m_width, 0);
	glEnd();
	CTextureManager::GetInstance()->SetTexture("");
	glColor3f(0.0f,0.0f,0.0f);
	int fontx = (m_width - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (unsigned char*)m_text.c_str())) / 2 ;
	int fonty = (m_height + 15) / 2;
	PrintText(fontx, fonty, m_text.c_str(), GLUT_BITMAP_HELVETICA_18);
	CUIElement::Draw();
	glPopMatrix();
}

bool CUIButton::LeftMouseButtonUp(int x, int y)
{
	if(!CUIElement::LeftMouseButtonUp(x, y))
	{
		m_isPressed = false;
		m_onClick();
	}
	return true;
}

bool CUIButton::LeftMouseButtonDown(int x, int y)
{
	CUIElement::LeftMouseButtonDown(x, y);
	m_isPressed = true;
	return true;
}