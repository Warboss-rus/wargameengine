#include "3dObject.h"
#include <GL\glut.h>

std::string C3DObject::GetPathToModel() const
{
	return m_model;
}

C3DObject::C3DObject(std::string const& model, double x, double y, double rotation):m_model(model), m_x(x), m_y(y), 
		m_rotation(rotation)
{
	m_bounding[0] = -0.8;
	m_bounding[1] = -0.5;
	m_bounding[2] = 0.0;
	m_bounding[3] = 0.8;
	m_bounding[4] = 0.5;
	m_bounding[5] = 2.8;
}