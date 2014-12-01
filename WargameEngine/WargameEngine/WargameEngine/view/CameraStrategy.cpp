#include "CameraStrategy.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../SoundPlayer.h"
#define TRANSLATE					  0.3
#define SCALE						  1.1
#define  GLUT_KEY_LEFT                      0x0064
#define  GLUT_KEY_UP                        0x0065
#define  GLUT_KEY_RIGHT                     0x0066
#define  GLUT_KEY_DOWN                      0x0067

CCameraStrategy::CCameraStrategy(double maxTransX, double maxTransY, double maxScale, double minScale)
	:m_maxTransX(maxTransX), m_maxTransY(maxTransY), m_maxScale(maxScale), m_minScale(minScale), m_hidePointer(false)
{
	Reset();
}

void CCameraStrategy::Reset()
{
	m_transX = 0.0;
	m_transY = 0.0;
	m_rotX = 60.0;
	m_rotZ = 0.0;
	m_scale = 1.0;
}

const double * CCameraStrategy::GetPosition() const
{
	static double pos[3];
	pos[0] = 0.0;
	pos[1] = 0.0;
	pos[2] = 10.0;
	//rotateX
	double temp1 = pos[1] * cos(-m_rotX * M_PI / 180) + pos[2] * sin(-m_rotX * M_PI / 180);
	pos[2] = pos[1] * sin(-m_rotX * M_PI / 180) + pos[2] * cos(-m_rotX * M_PI / 180);
	pos[1] = temp1;
	//rotateZ
	double temp = pos[0] * cos(m_rotZ * M_PI / 180) + pos[1] * sin(m_rotZ * M_PI / 180);
	pos[1] = pos[0] * sin(m_rotZ * M_PI / 180) + pos[1] * cos(m_rotZ * M_PI / 180);
	pos[0] = temp;
	//scale
	for (unsigned int i = 0; i < 3; ++i)
	{
		pos[i] /= m_scale;
	}
	//translate
	pos[0] += m_transX;
	pos[1] += m_transY;
	return pos;
}

const double * CCameraStrategy::GetDirection() const
{
	static double dir[3];
	dir[0] = m_transX;
	dir[1] = m_transY;
	dir[2] = 0.0;
	return dir;
}

const double * CCameraStrategy::GetUpVector() const
{
	static double up[3];
	up[0] = 0.0;
	up[1] = 0.0;
	up[2] = 1.0;
	//rotateZ
	double temp = up[0] * cos(m_rotZ * M_PI / 180) + up[1] * sin(m_rotZ * M_PI / 180);
	up[1] = up[0] * sin(m_rotZ * M_PI / 180) + up[1] * cos(m_rotZ * M_PI / 180);
	up[0] = temp;
	return up;
}

bool CCameraStrategy::OnSpecialKeyPress(int key)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
	{
		Translate(-TRANSLATE, 0.0);
		return true;
	}
	break;
	case GLUT_KEY_RIGHT:
	{
		Translate(TRANSLATE, 0.0);
		return true;
	}
	break;
	case GLUT_KEY_DOWN:
	{
		Translate(0.0, -TRANSLATE);
		return true;
	}
	break;
	case GLUT_KEY_UP:
	{
		Translate(0.0, TRANSLATE);
		return true;
	}
	break;
	}
	return false;
}

bool CCameraStrategy::OnMouseMove(int deltaX, int deltaY, bool LMB, bool RMB, bool shift, bool ctrl, bool alt)
{
	m_hidePointer = alt;
	if (alt)
	{
		Rotate(deltaX, deltaY);
	}
	return true;
}

void CCameraStrategy::Translate(double transX, double transY)
{
	m_transX += transX * cos(m_rotZ * M_PI / 180.0) + transY * sin(m_rotZ * M_PI / 180.0);
	if (m_transX > m_maxTransX) m_transX = m_maxTransX;
	if (m_transX < -m_maxTransX) m_transX = -m_maxTransX;
	m_transY += -transX * sin(m_rotZ * M_PI / 180.0) + transY * cos(m_rotZ * M_PI / 180.0);
	if (m_transY > m_maxTransY) m_transY = m_maxTransY;
	if (m_transY < -m_maxTransY) m_transY = -m_maxTransY;
}

void CCameraStrategy::Rotate(double rotZ, double rotX)
{
	m_rotZ = fmod(m_rotZ + rotZ, 360);
	m_rotX += rotX;
	if (m_rotX > 90.0) m_rotX = 90.0;
	if (m_rotX < 1.0) m_rotX = 1.0;
}

bool CCameraStrategy::OnMouseWheelUp()
{
	m_scale *= SCALE;
	if (m_scale > m_maxScale) m_scale = m_maxScale;
	return true;
}

bool CCameraStrategy::OnMouseWheelDown()
{
	m_scale *= 1 / SCALE;
	if (m_scale < m_minScale) m_scale = m_minScale;
	return true;
}