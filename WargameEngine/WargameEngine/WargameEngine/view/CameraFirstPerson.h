#pragma once
#include "ICamera.h"

class CCameraFirstPerson : public ICamera
{
public:
	CCameraFirstPerson();
	const double * GetPosition() const;
	const double * GetDirection() const;
	const double * GetUpVector() const;
	bool OnSpecialKeyPress(int key);
	bool OnMouseMove(int deltaX, int deltaY, bool LMB = false, bool RMB = false, bool shift = false, bool ctrl = false, bool alt = false);
	bool OnMouseWheelUp() { return false; }
	bool OnMouseWheelDown() { return false; }
	void Reset();
	const double GetScale() const { return 1.0; }
	bool HidePointer() const { return true; }
private:
	void Translate(double transX, double transY);
	void Rotate(double rotZ, double rotX);
	double m_transX;
	double m_transY;
	double m_transZ;
	double m_rotX;
	double m_rotZ;
};