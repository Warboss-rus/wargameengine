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
constexpr float SCALE = 1.1f;

CVector3f GetRotationsFromVector(const CVector3f& vec)
{
	auto r = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	auto t = atan(vec.y / vec.x);
	auto p = acos(vec.z / r);
	return CVector3f(r, t, p);
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
	return m_direction;
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
	m_direction = target;
	m_up = up;
}

void Camera::Rotate(float dx, float dy, float dz /*= 0.0f*/)
{
	auto rotations = GetRotationsFromVector(m_direction - m_position);
	rotations += CVector3f(dx, dy, dz);
	switch (m_cameraMode)
	{
	case Mode::THIRD_PERSON:
	{
		//Rotate position around target
		break;
	}
	case Mode::FIRST_PERSON:
	{
		//Rotate target around position
		break;
	}
	}
}

void Camera::Translate(float dx, float dy, float dz)
{
	auto rotations = GetRotationsFromVector(m_direction - m_position);
	float transY1 = dy * cos(rotations.x * PI / 180.0f);
	CVector3f delta;
	delta.z = dy * sin(rotations.x * PI / 180.0f) + dz;
	delta.x = dx * cos(rotations.z * PI / 180.0f) + transY1 * sin(rotations.z * PI / 180.0f);
	delta.y = -dx * sin(rotations.z * PI / 180.0f) + transY1 * cos(rotations.z * PI / 180.0f);
	m_position += delta;
	m_direction += delta;
}

void Camera::TranslateAbsolute(float dx, float dy, float dz)
{
	CVector3f delta(dx, dy, dz);
	m_position += delta;
	m_direction += delta;
}

void Camera::Scale(float multiplier)
{
	m_scale *= multiplier;
}

void Camera::SetCameraMode(Mode mode)
{
	m_cameraMode = mode;
}

void Camera::SetLimits(float maxTransX, float maxTransY, float minScale, float maxScale)
{
	m_minTransX = -maxTransX;
	m_maxTransX = maxTransX;
	m_minTransY = -maxTransY;
	m_maxTransY = maxTransY;
	m_minScale = minScale;
	m_maxScale = maxScale;
}

void Camera::AttachToTouchScreen()
{
	m_input->DoOnLMBDown([](int x, int y) {

		return false;
	}, CAMERA_PRIORITY, CAMERA_TAG);
}

void Camera::AttachToKeyboardMouse()
{
	m_input->DoOnMouseMove([this](int newX, int newY, int deltaX, int deltaY) {
		if ((m_cameraMode == Mode::FIRST_PERSON) || (m_input->GetModifiers() & IInput::MODIFIER_ALT))
		{
			Rotate(deltaX, deltaY);
		}
		return false;
	}, CAMERA_PRIORITY, CAMERA_TAG);
	m_input->DoOnKeyDown([this](VirtualKey virtualKey, int) {
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
		}
		return false;
	});
	m_input->DoOnMouseWheel([](int delta) {

		return false;
	});
}

void Camera::AttachToGamepad(size_t gamepadIndex)
{

}

void Camera::AttachToVR()
{

}

void Camera::ResetInput()
{
	m_input->DeleteAllSignalsByTag(CAMERA_TAG);
}
