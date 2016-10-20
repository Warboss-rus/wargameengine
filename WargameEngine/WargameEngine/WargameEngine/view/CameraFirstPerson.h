#pragma once
#include "ICamera.h"

class CCameraFirstPerson : public ICamera
{
public:
	CCameraFirstPerson();
	~CCameraFirstPerson();
	virtual CVector3d GetPosition() const override;
	virtual CVector3d GetDirection() const override;
	virtual CVector3d GetUpVector() const override;
	virtual const double GetScale() const override;
	virtual void SetInput(IInput & input) override;
	virtual void EnableTouchMode() override;
	void AttachVR(IInput & input);
private:
	void Reset();
	void Translate(double transX, double transY);
	void Rotate(double rotZ, double rotX);
	double m_transX;
	double m_transY;
	double m_transZ;
	double m_rotX;
	double m_rotY;
	double m_rotZ;
	IInput* m_input;
};