#include "Camera.h"
#include <GL\glut.h>
#include <math.h>

const double CCamera::SCALE = 1.1;
const double CCamera::TRANSLATE = 0.3;

void CCamera::Update()
{
	glLoadIdentity();
	glTranslated(m_transX, m_transY, -10.0);
	glScaled(m_scale, m_scale, m_scale);
	glRotated(m_rotX, -1.0, 0.0, 0.0);
	glRotated(m_rotZ, 0.0, 0.0, 1.0);
}

void CCamera::Translate(double transX, double transY)
{
	m_transX += transX;
	if(m_transX > m_maxTransX) m_transX = m_maxTransX;
	if(m_transX < -m_maxTransX) m_transX = -m_maxTransX;
	m_transY += transY;
	if(m_transY > m_maxTransY) m_transY = m_maxTransY;
	if(m_transY < -m_maxTransY) m_transY = -m_maxTransY;
}

void CCamera::Rotate(double rotZ, double rotX)
{
	m_rotZ = fmod(m_rotZ + rotZ, 360);
	m_rotX += rotX;
	if(m_rotX > 90) m_rotX = 90;
	if(m_rotX < 0.1) m_rotX= 0.1;
}

void CCamera::ZoomIn()
{
	m_scale *= SCALE;
	if(m_scale > m_maxScale) m_scale = m_maxScale;
}

void CCamera::ZoomOut()
{
	m_scale *= 1 / SCALE;
	if(m_scale < m_minScale) m_scale = m_minScale;
}

void CCamera::Reset()
{
	m_transX = 0;
	m_transY = 0;
	m_rotX = 60;
	m_rotZ = 0;
	m_scale = 1.0;
}