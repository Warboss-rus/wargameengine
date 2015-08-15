#pragma once
#include "ICamera.h"

class CCameraFirstPerson : public ICamera
{
public:
	CCameraFirstPerson();
	virtual const double * GetPosition() const override;
	virtual const double * GetDirection() const override;
	virtual const double * GetUpVector() const override;
	virtual bool OnKeyPress(int key) override;
	virtual bool OnMouseMove(int deltaX, int deltaY, bool LMB = false, bool RMB = false, bool shift = false, bool ctrl = false, bool alt = false) override;
	virtual bool OnMouseWheelUp() override;
	virtual bool OnMouseWheelDown() override;
	virtual const double GetScale() const override;
	virtual bool HidePointer() const override;
private:
	void Reset();
	void Translate(double transX, double transY);
	void Rotate(double rotZ, double rotX);
	double m_transX;
	double m_transY;
	double m_transZ;
	double m_rotX;
	double m_rotZ;
};