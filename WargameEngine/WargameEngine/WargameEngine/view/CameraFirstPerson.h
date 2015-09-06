#pragma once
#include "ICamera.h"

class CCameraFirstPerson : public ICamera
{
public:
	CCameraFirstPerson();
	~CCameraFirstPerson();
	virtual const double * GetPosition() const override;
	virtual const double * GetDirection() const override;
	virtual const double * GetUpVector() const override;
	virtual const double GetScale() const override;
	virtual void SetInput(IInput & input) override;
private:
	void Reset();
	void Translate(double transX, double transY);
	void Rotate(double rotZ, double rotX);
	double m_transX;
	double m_transY;
	double m_transZ;
	double m_rotX;
	double m_rotZ;
	IInput* m_input;
};