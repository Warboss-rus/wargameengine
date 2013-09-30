#include "3dObject.h"
#include <GL\glut.h>

void C3DObject::Draw() 
{
	glPushMatrix();
	glTranslated(m_x, m_y, 0);
	glRotated(m_rotation, 0.0, 0.0, 1.0);
	m_model->Draw(); 
	glPopMatrix();
}