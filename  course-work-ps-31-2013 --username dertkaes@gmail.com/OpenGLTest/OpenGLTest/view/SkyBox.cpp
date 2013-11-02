#include "SkyBox.h"
#include <GL\glut.h>
#include "TextureManager.h"

void CSkyBox::Draw(double x, double y, double z, double scale)
{
	x = -x - m_width / (scale * 2);
	y = -y - m_height / (scale * 2);
	z = -z - m_length / (scale * 2);
	CTextureManager * texManager = CTextureManager::GetInstance();
	// Top side
	texManager->SetTexture(m_imageFolder + "/top.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x, y, z + m_length / scale);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x, y + m_height / scale, z + m_length / scale);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x + m_width / scale, y + m_height / scale, z + m_length / scale); 
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x + m_width / scale, y, z + m_length / scale);
	glEnd();
	// Bottom side
	texManager->SetTexture(m_imageFolder + "/bottom.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x + m_width / scale, y, z);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x + m_width / scale, y + m_height / scale, z); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x, y + m_height / scale, z);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x, y, z);			
	glEnd();
	// Front side
	texManager->SetTexture(m_imageFolder + "/front.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x, y, z);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x,		  y,		z + m_length / scale);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x + m_width / scale, y,		z + m_length / scale); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x + m_width / scale, y,		z);	
	glEnd();
	// Back side
	texManager->SetTexture(m_imageFolder + "/back.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x, y + m_height / scale, z);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x,		  y + m_height / scale,		z + m_length / scale);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x + m_width / scale, y + m_height / scale,		z + m_length / scale); 
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x + m_width / scale, y + m_height / scale,		z);	
	glEnd();
	// Left side
	texManager->SetTexture(m_imageFolder + "/left.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x, y + m_height / scale, z);	
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x, y + m_height / scale, z + m_length / scale); 
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x, y, z + m_length / scale);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x, y, z);	
	glEnd();
	// Right side
	texManager->SetTexture(m_imageFolder + "/right.bmp");
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x + m_width / scale, y,		z);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x + m_width / scale, y,		z + m_length / scale);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x + m_width / scale, y + m_height / scale,	z + m_length / scale); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x + m_width / scale, y + m_height / scale,	z);
	glEnd();
	texManager->SetTexture("");
}
