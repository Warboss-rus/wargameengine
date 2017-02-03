#pragma once
#include "ICamera.h"

class CCameraFirstPerson : public ICamera
{
public:
	CCameraFirstPerson();
	~CCameraFirstPerson();
	virtual CVector3f GetPosition() const override;
	virtual CVector3f GetDirection() const override;
	virtual CVector3f GetUpVector() const override;
	virtual const float GetScale() const override;
	virtual void SetInput(IInput & input) override;
	virtual void EnableTouchMode() override;
	void AttachVR(IInput & input, int device = 0);
private:
	void Reset();
	void Translate(float transX, float transY);
	void Rotate(float rotZ, float rotX);
	float m_transX = 0.0f;
	float m_transY = 0.0f;
	float m_transZ = 0.0f;
	float m_rotX = 0.0f;
	float m_rotY = 0.0f;
	float m_rotZ = 0.0f;
	IInput* m_input = nullptr;
};