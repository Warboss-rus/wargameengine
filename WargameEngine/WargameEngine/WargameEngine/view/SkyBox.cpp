#include "SkyBox.h"
#include "../gl.h"
#include "TextureManager.h"

#ifndef GL_CLAMP_TO_EDGE_EXT 
#define GL_CLAMP_TO_EDGE_EXT 0x812F 
#endif 

CSkyBox::CSkyBox(double width, double height, double length, std::string const& imageFolder) : m_width(width), m_height(height), m_length(length), m_imageFolder(imageFolder)
{
	m_list = glGenLists(1);
	ResetList();
}

void CSkyBox::ResetList()
{
	glNewList(m_list, GL_COMPILE);
	CTextureManager * texManager = CTextureManager::GetInstance();
	// Top side
	texManager->SetTexture(m_imageFolder + "/top.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f); glVertex3d(0.0, 0.0, m_length);
	glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0, m_height, m_length);
	glTexCoord2f(0.0f, 0.0f); glVertex3d(m_width, m_height, m_length);
	glTexCoord2f(1.0f, 0.0f); glVertex3d(m_width, 0.0, m_length);
	glEnd();
	// Bottom side
	texManager->SetTexture(m_imageFolder + "/bottom.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f); glVertex3d(m_width, 0.0, 0.0);
	glTexCoord2f(0.0f, 1.0f); glVertex3d(m_width, m_height, 0.0);
	glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0, m_height, 0.0);
	glTexCoord2f(1.0f, 0.0f); glVertex3d(0.0, 0.0, 0.0);
	glEnd();
	// Front side
	texManager->SetTexture(m_imageFolder + "/front.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3d(0.0, 0.0, 0.0);
	glTexCoord2f(1.0f, 1.0f); glVertex3d(0.0, 0.0, m_length);
	glTexCoord2f(0.0f, 1.0f); glVertex3d(m_width, 0.0, m_length);
	glTexCoord2f(0.0f, 0.0f); glVertex3d(m_width, 0.0, 0.0);
	glEnd();
	// Back side
	texManager->SetTexture(m_imageFolder + "/back.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0, m_height, 0.0);
	glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0, m_height, m_length);
	glTexCoord2f(1.0f, 1.0f); glVertex3d(m_width, m_height, m_length);
	glTexCoord2f(1.0f, 0.0f); glVertex3d(m_width, m_height, 0.0);
	glEnd();
	// Left side
	texManager->SetTexture(m_imageFolder + "/left.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3d(0.0, m_height, 0.0);
	glTexCoord2f(1.0f, 1.0f); glVertex3d(0.0, m_height, m_length);
	glTexCoord2f(0.0f, 1.0f); glVertex3d(0.0, 0.0, m_length);
	glTexCoord2f(0.0f, 0.0f); glVertex3d(0.0, 0.0, 0.0);
	glEnd();
	// Right side
	texManager->SetTexture(m_imageFolder + "/right.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3d(m_width, 0.0, 0.0);
	glTexCoord2f(1.0f, 1.0f); glVertex3d(m_width, 0.0, m_length);
	glTexCoord2f(0.0f, 1.0f); glVertex3d(m_width, m_height, m_length);
	glTexCoord2f(0.0f, 0.0f); glVertex3d(m_width, m_height, 0.0);
	glEnd();
	texManager->SetTexture("");

	glEndList();
}

void CSkyBox::Draw(double x, double y, double z, double scale)
{
	glPushMatrix();
	x = -x - m_width / (scale * 2);
	y = -y - m_height / (scale * 2);
	z = -z - m_length / (scale * 2);
	glTranslated(x, y, z);
	glScaled(1 / scale, 1 / scale, 1 / scale);
	glCallList(m_list);
	glPopMatrix();
}

CSkyBox::~CSkyBox()
{
	glDeleteLists(m_list, 1);
}