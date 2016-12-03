#include "CameraFirstPerson.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define TRANSLATE					  0.3f
#include "KeyDefines.h"
#include <memory>

constexpr float PI = (float)M_PI;

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
		m_input->DeleteAllSignalsByTag(g_cameraTag);
	}
}

void CCameraFirstPerson::Reset()
{
	m_transX = 0.0;
	m_transY = 0.0;
	m_transZ = 1.0;
	m_rotX = 0.0;
	m_rotZ = 0.0;
}

CVector3f CCameraFirstPerson::GetPosition() const
{
	return { m_transX, m_transY, m_transZ };
}

CVector3f CCameraFirstPerson::GetDirection() const
{
	CVector3f pos = {0.0f, 10.0f, 0.0f};
	//rotateX
	float temp1 = pos.y * cos(m_rotX * PI / 180) + pos.z * sin(m_rotX * PI / 180);
	pos.z = pos.y * sin(m_rotX * PI / 180) + pos.z * cos(m_rotX * PI / 180);
	pos.y = temp1;
	//rotateZ
	float temp = pos.x * cos(m_rotZ * PI / 180) + pos.y * sin(m_rotZ * PI / 180);
	pos.y = pos.x * sin(m_rotZ * PI / 180) + pos.y * cos(m_rotZ * PI / 180);
	pos.x = temp;
	//translate
	pos.x += m_transX;
	pos.y += m_transY;
	pos.z += m_transZ;
	return pos;
}

CVector3f CCameraFirstPerson::GetUpVector() const
{
	CVector3f up = {0.0f, 0.0f, 1.0f};
	//rotateZ
	float temp = up[0] * cos(m_rotZ * PI / 180) + up[1] * sin(m_rotZ * PI / 180);
	up.y = up[0] * sin(m_rotZ * PI / 180) + up[1] * cos(m_rotZ * PI / 180);
	up.x = temp;
	return up;
}

const float CCameraFirstPerson::GetScale() const
{
	return 1.0;
}

void CCameraFirstPerson::Rotate(float rotZ, float rotX)
{
	m_rotZ = fmodf(m_rotZ + rotZ, 360);
	m_rotX = fmodf(m_rotX + rotX, 360);
}

void CCameraFirstPerson::SetInput(IInput & input)
{
	input.EnableCursor(false);
	int m_oldX = input.GetMouseX();
	int m_oldY = input.GetMouseY();
	input.DoOnMouseMove([=](int x, int y) {
		Rotate(static_cast<float>(x - m_oldX), static_cast<float>(y - m_oldY));
		return true;
	}, 1, g_cameraTag);
	input.DoOnKeyDown([this, &input](int key, int) {
		switch (input.KeycodeToVirtualKey(key))
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

void CCameraFirstPerson::EnableTouchMode()
{
	std::shared_ptr<CVector2i> lastCoords = std::make_shared<CVector2i>();
	m_input->DoOnLMBDown([lastCoords](int x, int y) {
		lastCoords->x = x;
		lastCoords->y = y;
		return false;
	}, 10, g_cameraTag);
	m_input->DoOnMouseMove([lastCoords, this](int x, int y) {
		if (lastCoords->x != 0 && lastCoords->y != 0)
		{
			m_rotX += (x - lastCoords->x) * 0.01f;
			m_rotZ += (y - lastCoords->y) * 0.01f;
			lastCoords->x = x;
			lastCoords->y = y;
		}
		return false;
	}, 10, g_cameraTag);
	m_input->DoOnLMBUp([lastCoords, this](int x, int y) {
		if (lastCoords->x != 0 && lastCoords->y != 0)
		{
			m_rotX += (x - lastCoords->x) * 0.01f;
			m_rotZ += (y - lastCoords->y) * 0.01f;
		}
		*lastCoords = CVector2i();
		return false;
	}, 10, g_cameraTag);
}

void CCameraFirstPerson::AttachVR(IInput & input, int device)
{
	input.DoOnHeadRotationChange([this, device] (int deviceIndex, float x, float y, float z){
		if (deviceIndex == device)
		{
			m_rotX = x;
			m_rotY = y;
			m_rotZ = z;
			return true;
		}
		return false;
	});
}

void CCameraFirstPerson::Translate(float transX, float transY)
{
	float transY1 = transY * cos(m_rotX * PI / 180.0f);
	m_transZ += transY * sin(m_rotX * PI / 180.0f);
	m_transX += transX * cos(m_rotZ * PI / 180.0f) + transY1 * sin(m_rotZ * PI / 180.0f);
	m_transY += -transX * sin(m_rotZ * PI / 180.0f) + transY1 * cos(m_rotZ * PI / 180.0f);
}