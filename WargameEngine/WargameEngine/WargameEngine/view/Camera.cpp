#include "Camera.h"
#include "IInput.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace
{
constexpr float PI = static_cast<float>(M_PI);
constexpr char CAMERA_TAG[] = "camera";
constexpr int CAMERA_PRIORITY = 0;
constexpr float TRANSLATE = 0.3f;
float clamp(float value, float min, float max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}
}

Camera::Camera(IInput& input)
	: m_input(&input)
{
}

Camera::~Camera()
{
	ResetInput();
}

CVector3f Camera::GetPosition() const
{
	return m_position;
}

CVector3f Camera::GetDirection() const
{
	return m_target;
}

CVector3f Camera::GetUpVector() const
{
	return m_up;
}

const float Camera::GetScale() const
{
	return m_scale;
}

void Camera::Set(const CVector3f& position, const CVector3f& target /*= CVector3f()*/, const CVector3f& up /*= CVector3f(0.0f, 0.0f, 1.0f)*/)
{
	m_position = position;
	m_target = target;
	m_up = up;
}

void Camera::Rotate(float dx, float dy, float dz)
{
	auto deltaVec = m_target - m_position;
	float r = deltaVec.GetLength();
	auto t = atan(deltaVec.y / deltaVec.x) * 180.0f / PI;
	auto p = acos(deltaVec.z / deltaVec.GetLength()) * 180.0f / PI;
	t += dx;
	p += dz;
	t *= PI / 180.0f;
	p *= PI / 180.0f;
	deltaVec.x = r * sin(p) * cos(t);
	deltaVec.y = r * sin(p) * sin(t);
	deltaVec.z = r * cos(p);
	switch (m_cameraMode)
	{
	case Mode::THIRD_PERSON:
	{
		m_position = m_target - deltaVec;
		break;
	}
	case Mode::FIRST_PERSON:
	{
		//Rotate target around position
		//rotateX
		/*float temp = pos.y * cos(dx * PI / 180) + pos.z * sin(dx * PI / 180);
		pos.z = pos.y * sin(dx * PI / 180) + pos.z * cos(dx * PI / 180);
		pos.y = temp;
		//rotateZ
		temp = pos.x * cos(dz * PI / 180) + pos.y * sin(dz * PI / 180);
		pos.y = pos.x * sin(dz * PI / 180) + pos.y * cos(dz * PI / 180);
		pos.x = temp;
		m_target = m_position + pos;*/
		break;
	}
	}
}

void Camera::Translate(float dx, float dy, float dz)
{
	auto deltaVec = m_target - m_position;
	float r = deltaVec.GetLength();
	auto t = atan(deltaVec.y / deltaVec.x);
	auto p = acos(deltaVec.z / deltaVec.GetLength());
	CVector3f delta(dx, dy, dz);
	delta.z = dy * sin(p) + dz;
	delta.x = dx * cos(t) + dy * sin(t);
	delta.y = -dx * sin(t) + dy * cos(t);
	TranslateAbsolute(delta);
}

void Camera::TranslateAbsolute(const CVector3f& delta)
{
	if (m_cameraMode == Mode::FIRST_PERSON)
	{
		CVector3f oldPosition = m_position;
		m_position += delta;
		m_position.x = clamp(m_position.x, m_minTransX, m_maxTransX);
		m_position.y = clamp(m_position.y, m_minTransY, m_maxTransY);
		m_position.z = clamp(m_position.z, m_minTransZ, m_maxTransZ);
		m_target += m_position - oldPosition;
	}
	else
	{
		CVector3f oldDirection = m_target;
		m_target += delta;
		m_target.x = clamp(m_target.x, m_minTransX, m_maxTransX);
		m_target.y = clamp(m_target.y, m_minTransY, m_maxTransY);
		m_target.z = clamp(m_target.z, m_minTransZ, m_maxTransZ);
		m_position += m_target - oldDirection;
	}
}

void Camera::Scale(float multiplier)
{
	m_scale *= multiplier;
	m_scale = clamp(m_scale, m_minScale, m_maxScale);
}

void Camera::SetCameraMode(Mode mode)
{
	m_cameraMode = mode;
}

void Camera::SetLimits(float maxTransX, float maxTransY, float maxTransZ, float minScale, float maxScale)
{
	m_minTransX = -maxTransX;
	m_maxTransX = maxTransX;
	m_minTransY = -maxTransY;
	m_maxTransY = maxTransY;
	m_minTransZ = 0;
	m_maxTransZ = maxTransZ;
	m_minScale = minScale;
	m_maxScale = maxScale;
}

void Camera::SetMouseSensitivity(float horizontalSensitivity, float verticalSensitivity)
{
	m_mouseHorizontalSensitivity = horizontalSensitivity;
	m_mouseVerticalSensitivity = verticalSensitivity;
}

void Camera::AttachToTouchScreen()
{
	m_inputConnections.emplace_back(m_input->DoOnMouseMove([this](int, int, int dx, int dy) {
		if (m_input->IsLMBPressed())
		{
			Translate(dx / 10.0f, dy / 10.0f, 0.0f);
		}
		return false;
	}, CAMERA_PRIORITY, CAMERA_TAG));
}

void Camera::AttachToKeyboardMouse()
{
	m_inputConnections.emplace_back(m_input->DoOnMouseMove([this](int /*newX*/, int /*newY*/, int deltaX, int deltaY) {
		if ((m_cameraMode == Mode::FIRST_PERSON) || (m_input->GetModifiers() & IInput::MODIFIER_ALT))
		{
			Rotate(deltaX * m_mouseHorizontalSensitivity, 0.0f, deltaY * m_mouseVerticalSensitivity);
		}
		return false;
	}, CAMERA_PRIORITY, CAMERA_TAG));
	m_inputConnections.emplace_back(m_input->DoOnKeyDown([this](VirtualKey virtualKey, int) {
		switch (virtualKey)
		{
		case VirtualKey::KEY_LEFT:
			Translate(-TRANSLATE, 0.0f, 0.0f);
			break;
		case VirtualKey::KEY_RIGHT:
			Translate(TRANSLATE, 0.0f, 0.0f);
			break;
		case VirtualKey::KEY_DOWN:
			Translate(0.0f, -TRANSLATE, 0.0f);
			break;
		case VirtualKey::KEY_UP:
			Translate(0.0f, TRANSLATE, 0.0f);
			break;
		default:
			break;
		}
		return false;
	}));
	m_inputConnections.emplace_back(m_input->DoOnMouseWheel([this](float delta) {
		Scale(1.0f - delta * 0.1f);
		return false;
	}));
}

void Camera::AttachToGamepad(size_t gamepadIndex)
{

}

void Camera::AttachToVR()
{

}

void Camera::ResetInput()
{
	m_inputConnections.clear();
}
