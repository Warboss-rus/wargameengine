#pragma once
#include "Vector3.h"
#include <chrono>
#include <vector>

class IInput;

class Camera
{
public:
	enum class Mode
	{
		THIRD_PERSON,
		FIRST_PERSON,
	};

	Camera(IInput& input);
	Camera(const Camera& other) = delete;
	Camera(Camera&& other) = default;
	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other) = default;
	~Camera();

	CVector3f GetPosition() const;
	CVector3f GetDirection() const;
	CVector3f GetUpVector() const;
	const float GetScale() const;

	void Set(const CVector3f& position, const CVector3f& target = CVector3f(), const CVector3f& up = CVector3f(0.0f, 0.0f, 1.0f));
	void Rotate(float dx, float dy, float dz = 0.0f);
	void Translate(float dx, float dy, float dz);
	void TranslateAbsolute(float dx, float dy, float dz);
	void Scale(float multiplier);
	void SetCameraMode(Mode mode);
	void SetLimits(float maxTransX, float maxTransY, float minScale, float maxScale);

	void AttachToTouchScreen();
	void AttachToKeyboardMouse();
	void AttachToGamepad(size_t gamepadIndex);
	void AttachToVR();
	void ResetInput();

private:
	IInput* m_input;
	CVector3f m_position = { 0.0f, 10.0f, 10.0f };
	CVector3f m_direction = { 0.0f, 0.0f, 0.0f };
	CVector3f m_up = { 0.0f, 0.0f, 1.0f };
	float m_scale = 1.0f;
	Mode m_cameraMode = Mode::THIRD_PERSON;
	float m_minTransX = -100.0f;
	float m_maxTransX = 100.0f;
	float m_minTransY = -100.0f;
	float m_maxTransY = 100.0f;
	float m_minScale = 0.5f;
	float m_maxScale = 2.0f;
	struct CameraTarget
	{
		CVector3f position;
		CVector3f direction;
		std::chrono::milliseconds time;
	};
	std::vector<CameraTarget> m_cameraPath;
	bool m_mouseButtonPressed = false;
};