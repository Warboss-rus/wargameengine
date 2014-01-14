#include "SkyBox.h"
#include "gl.h"
#include "TextureManager.h"

#ifndef GL_CLAMP_TO_EDGE_EXT 
#define GL_CLAMP_TO_EDGE_EXT 0x812F 
#endif 

CSkyBox::CSkyBox(float width, float height, float length, std::string const& imageFolder): m_width(width), m_height(height), m_length(length), m_imageFolder(imageFolder) 
{
}

void CSkyBox::Draw(float x, float y, float z, float scale)
{
	x = -x - m_width / (scale * 2);
	y = -y - m_height / (scale * 2);
	z = -z - m_length / (scale * 2);
	CTextureManager * texManager = CTextureManager::GetInstance();
	// Top side
	texManager->SetTexture(m_imageFolder + "/top.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x, y, z + m_length / scale);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x, y + m_height / scale, z + m_length / scale);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x + m_width / scale, y + m_height / scale, z + m_length / scale); 
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x + m_width / scale, y, z + m_length / scale);
	glEnd();
	// Bottom side
	texManager->SetTexture(m_imageFolder + "/bottom.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x + m_width / scale, y, z);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x + m_width / scale, y + m_height / scale, z); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x, y + m_height / scale, z);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x, y, z);			
	glEnd();
	// Front side
	texManager->SetTexture(m_imageFolder + "/front.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x, y, z);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x,		  y,		z + m_length / scale);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x + m_width / scale, y,		z + m_length / scale); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x + m_width / scale, y,		z);	
	glEnd();
	// Back side
	texManager->SetTexture(m_imageFolder + "/back.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x, y + m_height / scale, z);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x,		  y + m_height / scale,		z + m_length / scale);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x + m_width / scale, y + m_height / scale,		z + m_length / scale); 
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x + m_width / scale, y + m_height / scale,		z);	
	glEnd();
	// Left side
	texManager->SetTexture(m_imageFolder + "/left.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x, y + m_height / scale, z);	
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x, y + m_height / scale, z + m_length / scale); 
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x, y, z + m_length / scale);
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x, y, z);	
	glEnd();
	// Right side
	texManager->SetTexture(m_imageFolder + "/right.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(x + m_width / scale, y,		z);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(x + m_width / scale, y,		z + m_length / scale);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(x + m_width / scale, y + m_height / scale,	z + m_length / scale); 
		glTexCoord2f(0.0f, 0.0f); glVertex3d(x + m_width / scale, y + m_height / scale,	z);
	glEnd();
	texManager->SetTexture("");
}
