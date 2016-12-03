#pragma once
#include "ICamera.h"

class CCameraStrategy : public ICamera
{
public:
	CCameraStrategy(float maxTransX, float maxTransY, float maxScale, float minScale);
	~CCameraStrategy();
	virtual CVector3f GetPosition() const override;
	virtual CVector3f GetDirection() const override;
	virtual CVector3f GetUpVector() const override;
	virtual void SetInput(IInput & input) override;
	virtual const float GetScale() const override;
	virtual void EnableTouchMode() override;
private:
	void Reset();
	void Translate(float transX, float transY);
	void Rotate(float rotZ, float rotX);

	const float m_maxTransX;
	const float m_maxTransY;
	const float m_maxScale;
	const float m_minScale;
	float m_transX;
	float m_transY;
	float m_rotX;
	float m_rotZ;
	float m_scale;
	bool m_hidePointer;
	int m_oldX;
	int m_oldY;
	IInput * m_input;
};