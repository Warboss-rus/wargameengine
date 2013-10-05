#include "3dObject.h"
#include <GL\glut.h>

void C3DObject::Draw() const
{
	glPushMatrix();
	glTranslated(m_x, m_y, 0);
	glRotated(m_rotation, 0.0, 0.0, 1.0);
	m_model->Draw(); 
	glPopMatrix();
}

C3DObject::C3DObject(C3DModel * model, double x, double y, double rotation):m_model(model), m_x(x), m_y(y), 
		m_rotation(rotation)
{
	m_bounding[0] = -0.8;
	m_bounding[1] = 0.0;
	m_bounding[2] = -0.5;
	m_bounding[3] = 0.8;
	m_bounding[4] = 2.8;
	m_bounding[5] = 0.5;
}