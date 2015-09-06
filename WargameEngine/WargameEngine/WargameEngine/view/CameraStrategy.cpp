#include "CameraStrategy.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define TRANSLATE					  0.3
#define SCALE						  1.1
#include "KeyDefines.h"

CCameraStrategy::CCameraStrategy(double maxTransX, double maxTransY, double maxScale, double minScale)
	:m_maxTransX(maxTransX), m_maxTransY(maxTransY), m_maxScale(maxScale), m_minScale(minScale), m_hidePointer(false), m_input(nullptr)
{
	Reset();
}

static const std::string g_cameraTag = "camera";

CCameraStrategy::~CCameraStrategy()
{
	if (m_input)
	{
		m_input->DeleteAllSignalsByTag(g_cameraTag);
	}
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
	for (size_t i = 0; i < 3; ++i)
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

void CCameraStrategy::SetInput(IInput & input)
{
	m_input = &input;
	input.DoOnMouseWheelUp([this]() {
		m_scale *= SCALE;
		if (m_scale > m_maxScale) m_scale = m_maxScale;
		return true;
	}, 1, g_cameraTag);
	input.DoOnMouseWheelDown([this]() {
		m_scale *= 1 / SCALE;
		if (m_scale < m_minScale) m_scale = m_minScale;
		return true;
	}, 1, g_cameraTag);
	input.DoOnMouseMove([this](int x, int y) {
		int modifiers = m_input->GetModifiers();
		if (m_hidePointer)
		{
			Rotate(x - m_oldX, m_oldY - y);
			if (!(modifiers & IInput::MODIFIER_ALT))
			{
				m_hidePointer = false;
				m_input->EnableCursor(true);
			}
			return true;
		}
		else
		{
			if (modifiers & IInput::MODIFIER_ALT)
			{
				m_hidePointer = true;
				m_input->EnableCursor(false);
				m_oldX = m_input->GetMouseX();
				m_oldY = m_input->GetMouseY();
			}
		}
		return false;
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

const double CCameraStrategy::GetScale() const
{
	return m_scale;
}
