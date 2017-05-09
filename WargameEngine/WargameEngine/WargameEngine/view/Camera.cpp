#include "Camera.h"
#include "IInput.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Matrix4.h"

namespace
{
constexpr float PI = static_cast<float>(M_PI);
constexpr char CAMERA_TAG[] = "camera";
constexpr int CAMERA_PRIORITY = 1;
constexpr float TRANSLATE = 0.3f;

float clamp(float value, float min, float max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

CVector3f operator*(const CVector3f& vect, const Matrix4F& matrix)
{
	float result[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i] += matrix[i * 4 + j] * ((j == 3) ? 1.0f : vect[j]);
		}
	}
	if (fabs(result[3]) > FLT_EPSILON)
	{
		for (int i = 0; i < 3; ++i)
		{
			result[i] /= result[3];
		}
	}
	return CVector3f(result);
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
	if (m_vrDevice != -1)
	{
		return m_position * Matrix4F(m_input->GetHeadTrackingMatrix(m_vrDevice));
	}
	return m_position;
}

CVector3f Camera::GetDirection() const
{
	if (m_vrDevice != -1)
	{
		return m_target * Matrix4F(m_input->GetHeadTrackingMatrix(m_vrDevice));
	}
	return m_target;
}

CVector3f Camera::GetUpVector() const
{
	if (m_vrDevice != -1)
	{
		return m_up * Matrix4F(m_input->GetHeadTrackingMatrix(m_vrDevice));
	}
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
	auto t = atan2(deltaVec.y, deltaVec.x);
	auto p = acos(deltaVec.z / deltaVec.GetLength());
	t += dx * PI / 180.0f;
	p += dz * PI / 180.0f;
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
		m_target = deltaVec + m_position;
		break;
	}
	}
}

void Camera::Translate(float dx, float dy, float dz)
{
	auto deltaVec = m_target - m_position;
	float r = deltaVec.GetLength();
	auto t = atan2(deltaVec.y, deltaVec.x);
	auto p = acos(deltaVec.z / deltaVec.GetLength());
	CVector3f delta(dx, dy, dz);
	delta.x = dx * sin(t) + dy * cos(t);
	delta.y = dx * cos(t) + dy * sin(t);
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

void Camera::ChangeDistance(float delta)
{
	if (m_cameraMode == Mode::THIRD_PERSON)
	{
		auto pos = m_position - m_target;
		float length = pos.GetLength();
		float newLength = clamp(length + delta, m_minDistance, m_maxDistance);
		pos *= 1 + (newLength - length) / length;
		m_position = m_target + pos;
	}
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
	ResetInput();
	m_inputConnections.emplace_back(m_input->DoOnMouseMove([this](int, int, int dx, int dy) {
		if (m_input->IsLMBPressed())
		{
			Translate(-dx / 100.0f, dy / 100.0f, 0.0f);
		}
		return false;
	}, CAMERA_PRIORITY, CAMERA_TAG));
	m_inputConnections.emplace_back(m_input->DoOnMouseWheel([this](float multiplier) {
		ChangeDistance((m_position - m_target).GetLength() * (1.0f - multiplier));
		return false;
	}));
}

void Camera::AttachToKeyboardMouse()
{
	ResetInput();
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
		ChangeDistance(-delta);
		return false;
	}));
}

void Camera::AttachToGamepad(size_t gamepadIndex)
{

}

void Camera::AttachToVR(size_t deviceIndex)
{
	m_vrDevice = deviceIndex;
}

void Camera::ResetInput()
{
	m_inputConnections.clear();
	m_vrDevice = -1;
}
