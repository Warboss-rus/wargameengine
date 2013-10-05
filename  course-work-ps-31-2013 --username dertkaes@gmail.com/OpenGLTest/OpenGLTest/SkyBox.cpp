#include "SkyBox.h"
#include <GL\glut.h>
#include "TextureManager.h"
#define SKYBOX_TOP 0
#define SKYBOX_BOTTOM 1
#define SKYBOX_FRONT 2
#define SKYBOX_BACK 3
#define SKYBOX_LEFT 4
#define SKYBOX_RIGHT 5

void CSkyBox::Draw()
{
	CTextureManager * texManager = CTextureManager::GetInstance();
	if(m_textureID[SKYBOX_TOP] == 0)
	{
		m_textureID[SKYBOX_TOP] = texManager->GetTexture(m_imageFolder + "/top.bmp");
	}
	if(m_textureID[SKYBOX_BOTTOM] == 0)
	{
		m_textureID[SKYBOX_BOTTOM] = texManager->GetTexture(m_imageFolder + "/bottom.bmp");
	}
	if(m_textureID[SKYBOX_FRONT] == 0)
	{
		m_textureID[SKYBOX_FRONT] = texManager->GetTexture(m_imageFolder + "/front.bmp");
	}
	if(m_textureID[SKYBOX_BACK] == 0)
	{
		m_textureID[SKYBOX_BACK] = texManager->GetTexture(m_imageFolder + "/back.bmp");
	}
	if(m_textureID[SKYBOX_LEFT] == 0)
	{
		m_textureID[SKYBOX_LEFT] = texManager->GetTexture(m_imageFolder + "/left.bmp");
	}
	if(m_textureID[SKYBOX_RIGHT] == 0)
	{
		m_textureID[SKYBOX_RIGHT] = texManager->GetTexture(m_imageFolder + "/right.bmp");
	}
	glCullFace(GL_FRONT);
	// Top side
	glBindTexture(GL_TEXTURE_2D, m_textureID[SKYBOX_TOP]);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x + m_width, m_y + m_height, m_z);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x + m_width, m_y + m_height, m_z + m_length); 
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x, m_y + m_height,	m_z + m_length);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x, m_y + m_height,	m_z);
	glEnd();

	// Bottom side
	glBindTexture(GL_TEXTURE_2D, m_textureID[SKYBOX_BOTTOM]);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x, m_y, m_z);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x, m_y, m_z + m_length);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x + m_width, m_y, m_z + m_length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x + m_width, m_y, m_z);
	glEnd();

	// Front side
	glBindTexture(GL_TEXTURE_2D, m_textureID[SKYBOX_FRONT]);
	glBegin(GL_QUADS);	
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x, m_y, m_z + m_length);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x, m_y + m_height, m_z + m_length);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x + m_width, m_y + m_height, m_z + m_length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x + m_width, m_y, m_z + m_length);
	glEnd();

	// Back side
	glBindTexture(GL_TEXTURE_2D, m_textureID[SKYBOX_BACK]);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x + m_width, m_y, m_z);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x + m_width, m_y + m_height, m_z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x, m_y + m_height, m_z);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x, m_y, m_z);
	glEnd();

	// Left side
	glBindTexture(GL_TEXTURE_2D, m_textureID[SKYBOX_LEFT]);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x, m_y + m_height, m_z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x, m_y + m_height, m_z + m_length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x, m_y, m_z + m_length);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x, m_y, m_z);		
	glEnd();

	// Right side
	glBindTexture(GL_TEXTURE_2D, m_textureID[SKYBOX_RIGHT]);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3d(m_x + m_width, m_y, m_z);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(m_x + m_width, m_y, m_z + m_length);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(m_x + m_width, m_y + m_height,	m_z + m_length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3d(m_x + m_width, m_y + m_height,	m_z);
	glEnd();
	glCullFace(GL_BACK);
}
