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
	float m_transX;
	float m_transY;
	float m_transZ;
	float m_rotX;
	float m_rotY;
	float m_rotZ;
	IInput* m_input;
};