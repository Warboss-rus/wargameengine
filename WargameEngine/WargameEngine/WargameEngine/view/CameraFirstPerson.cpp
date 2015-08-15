#include "CameraFirstPerson.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define TRANSLATE					  0.3
#include "KeyDefines.h"

CCameraFirstPerson::CCameraFirstPerson()
{
	Reset();
}

void CCameraFirstPerson::Reset()
{
	m_transX = 0.0;
	m_transY = 0.0;
	m_transZ = 1.0;
	m_rotX = 0.0;
	m_rotZ = 0.0;
}

const double * CCameraFirstPerson::GetPosition() const
{
	static double dir[3];
	dir[0] = m_transX;
	dir[1] = m_transY;
	dir[2] = m_transZ;
	return dir;
}

const double * CCameraFirstPerson::GetDirection() const
{
	static double pos[3];
	pos[0] = 0.0;
	pos[1] = 10.0;
	pos[2] = 0.0;
	//rotateX
	double temp1 = pos[1] * cos(m_rotX * M_PI / 180) + pos[2] * sin(m_rotX * M_PI / 180);
	pos[2] = pos[1] * sin(m_rotX * M_PI / 180) + pos[2] * cos(m_rotX * M_PI / 180);
	pos[1] = temp1;
	//rotateZ
	double temp = pos[0] * cos(m_rotZ * M_PI / 180) + pos[1] * sin(m_rotZ * M_PI / 180);
	pos[1] = pos[0] * sin(m_rotZ * M_PI / 180) + pos[1] * cos(m_rotZ * M_PI / 180);
	pos[0] = temp;
	//translate
	pos[0] += m_transX;
	pos[1] += m_transY;
	pos[2] += m_transZ;
	return pos;
}

const double * CCameraFirstPerson::GetUpVector() const
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

bool CCameraFirstPerson::OnMouseMove(int deltaX, int deltaY, bool /*LMB*/, bool /*RMB*/, bool /*shift*/, bool /*ctrl*/, bool /*alt*/)
{
	Rotate(deltaX, deltaY);
	return true;
}

bool CCameraFirstPerson::OnMouseWheelUp()
{
	return false;
}

bool CCameraFirstPerson::OnMouseWheelDown()
{
	return false;
}

const double CCameraFirstPerson::GetScale() const
{
	return 1.0;
}

bool CCameraFirstPerson::HidePointer() const
{
	return true;
}

void CCameraFirstPerson::Rotate(double rotZ, double rotX)
{
	m_rotZ = fmod(m_rotZ + rotZ, 360);
	m_rotX = fmod(m_rotX + rotX, 360);
}

void CCameraFirstPerson::Translate(double transX, double transY)
{
	double transY1 = transY * cos(m_rotX * M_PI / 180.0);
	m_transZ += transY * sin(m_rotX * M_PI / 180.0);
	m_transX += transX * cos(m_rotZ * M_PI / 180.0) + transY1 * sin(m_rotZ * M_PI / 180.0);
	m_transY += -transX * sin(m_rotZ * M_PI / 180.0) + transY1 * cos(m_rotZ * M_PI / 180.0);
}

bool CCameraFirstPerson::OnKeyPress(int key)
{
	switch (key)
	{
	case KEY_LEFT:
	{
		Translate(-TRANSLATE, 0.0);
	}break;
	case KEY_RIGHT:
	{
		Translate(TRANSLATE, 0.0);
	}break;
	case KEY_DOWN:
	{
		Translate(0.0, -TRANSLATE);
	}
	break;
	case KEY_UP:
	{
		Translate(0.0, TRANSLATE);
	}break;
	case KEY_BACKSPACE:
	{
		Reset();
	}break;
	default:
	{
		return false;
	}
	}
	return true;
}