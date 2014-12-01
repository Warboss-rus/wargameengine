#pragma once
#include "ICamera.h"

class CCameraStrategy : public ICamera
{
public:
	CCameraStrategy(double maxTransX, double maxTransY, double maxScale, double minScale);
	const double * GetPosition() const;
	const double * GetDirection() const;
	const double * GetUpVector() const;
	bool OnSpecialKeyPress(int key);
	bool OnMouseMove(int deltaX, int deltaY, bool LMB = false, bool RMB = false, bool shift = false, bool ctrl = false, bool alt = false);
	bool OnMouseWheelUp();
	bool OnMouseWheelDown();
	void Reset();
	const double GetScale() const { return m_scale; }
	bool HidePointer() const { return m_hidePointer; }
private:
	void Translate(double transX, double transY);
	void Rotate(double rotZ, double rotX);
	const double m_maxTransX;
	const double m_maxTransY;
	const double m_maxScale;
	const double m_minScale;
	double m_transX;
	double m_transY;
	double m_rotX;
	double m_rotZ;
	double m_scale;
	bool m_hidePointer;
};