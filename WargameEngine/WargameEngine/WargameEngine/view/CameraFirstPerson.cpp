#include "CameraFirstPerson.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define TRANSLATE					  0.3
#include "KeyDefines.h"

CCameraFirstPerson::CCameraFirstPerson()
	:m_input(nullptr)
{
	Reset();
}

static const std::string g_cameraTag = "camera";

CCameraFirstPerson::~CCameraFirstPerson()
{
	if (m_input)
	{
		m_input->EnableCursor(true);
	}
	m_input->DeleteAllSignalsByTag(g_cameraTag);
}

void CCameraFirstPerson::Reset()
{
	m_transX = 0.0;
	m_transY = 0.0;
	m_transZ = 1.0;
	m_rotX = 0.0;
	m_rotZ = 0.0;
}

CVector3d CCameraFirstPerson::GetPosition() const
{
	return { m_transX, m_transY, m_transZ };
}

CVector3d CCameraFirstPerson::GetDirection() const
{
	CVector3d pos;
	pos.x = 0.0;
	pos.y = 10.0;
	pos.z = 0.0;
	//rotateX
	double temp1 = pos.y * cos(m_rotX * M_PI / 180) + pos.z * sin(m_rotX * M_PI / 180);
	pos.z = pos.y * sin(m_rotX * M_PI / 180) + pos.z * cos(m_rotX * M_PI / 180);
	pos.y = temp1;
	//rotateZ
	double temp = pos.x * cos(m_rotZ * M_PI / 180) + pos.y * sin(m_rotZ * M_PI / 180);
	pos.y = pos.x * sin(m_rotZ * M_PI / 180) + pos.y * cos(m_rotZ * M_PI / 180);
	pos.x = temp;
	//translate
	pos.x += m_transX;
	pos.y += m_transY;
	pos.z += m_transZ;
	return pos;
}

CVector3d CCameraFirstPerson::GetUpVector() const
{
	CVector3d up;
	up.x = 0.0;
	up.y = 0.0;
	up.z = 1.0;
	//rotateZ
	double temp = up[0] * cos(m_rotZ * M_PI / 180) + up[1] * sin(m_rotZ * M_PI / 180);
	up.y = up[0] * sin(m_rotZ * M_PI / 180) + up[1] * cos(m_rotZ * M_PI / 180);
	up.x = temp;
	return up;
}

const double CCameraFirstPerson::GetScale() const
{
	return 1.0;
}

void CCameraFirstPerson::Rotate(double rotZ, double rotX)
{
	m_rotZ = fmod(m_rotZ + rotZ, 360);
	m_rotX = fmod(m_rotX + rotX, 360);
}

void CCameraFirstPerson::SetInput(IInput & input)
{
	input.EnableCursor(false);
	int m_oldX = input.GetMouseX();
	int m_oldY = input.GetMouseY();
	input.DoOnMouseMove([=](int x, int y) {
		Rotate(x - m_oldX, y - m_oldY);
		return true;
	}, 1, g_cameraTag);
	input.DoOnKeyDown([this](int key, int) {
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
	}, 1, g_cameraTag);
}

void CCameraFirstPerson::Translate(double transX, double transY)
{
	double transY1 = transY * cos(m_rotX * M_PI / 180.0);
	m_transZ += transY * sin(m_rotX * M_PI / 180.0);
	m_transX += transX * cos(m_rotZ * M_PI / 180.0) + transY1 * sin(m_rotZ * M_PI / 180.0);
	m_transY += -transX * sin(m_rotZ * M_PI / 180.0) + transY1 * cos(m_rotZ * M_PI / 180.0);
}