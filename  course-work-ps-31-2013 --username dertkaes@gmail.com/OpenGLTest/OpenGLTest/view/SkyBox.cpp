#include "SkyBox.h"
#include <GL\glut.h>
#include "TextureManager.h"

void CSkyBox::Draw()
{
	CTextureManager * texManager = CTextureManager::GetInstance();
	glDisable(GL_DEPTH_TEST);
	// Top side
	texManager->SetTexture(m_imageFolder + "/top.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x, m_y, m_z + m_length);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x, m_y + m_height, m_z + m_length);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x + m_width, m_y + m_height, m_z + m_length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x + m_width, m_y, m_z + m_length);
	glEnd();
	// Bottom side
	texManager->SetTexture(m_imageFolder + "/bottom.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x + m_width, m_y, m_z);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x + m_width, m_y + m_height, m_z); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x, m_y + m_height, m_z);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x, m_y, m_z);			
	glEnd();
	// Front side
	texManager->SetTexture(m_imageFolder + "/front.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x, m_y, m_z);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x,		  m_y,		m_z + m_length);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x + m_width, m_y,		m_z + m_length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x + m_width, m_y,		m_z);	
	glEnd();
	// Back side
	texManager->SetTexture(m_imageFolder + "/back.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x, m_y + m_height, m_z);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x,		  m_y + m_height,		m_z + m_length);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x + m_width, m_y + m_height,		m_z + m_length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x + m_width, m_y + m_height,		m_z);	
	glEnd();
	// Left side
	texManager->SetTexture(m_imageFolder + "/left.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x, m_y + m_height, m_z);	
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x, m_y + m_height, m_z + m_length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x, m_y, m_z + m_length);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x, m_y, m_z);	
	glEnd();
	// Right side
	texManager->SetTexture(m_imageFolder + "/right.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x + m_width, m_y,		m_z);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x + m_width, m_y,		m_z + m_length);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x + m_width, m_y + m_height,	m_z + m_length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x + m_width, m_y + m_height,	m_z);
	glEnd();
	glEnable(GL_DEPTH_TEST);
}
