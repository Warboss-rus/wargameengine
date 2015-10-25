#pragma once
#include "ICamera.h"

class CCameraStrategy : public ICamera
{
public:
	CCameraStrategy(double maxTransX, double maxTransY, double maxScale, double minScale);
	~CCameraStrategy();
	virtual CVector3d GetPosition() const override;
	virtual CVector3d GetDirection() const override;
	virtual CVector3d GetUpVector() const override;
	virtual void SetInput(IInput & input) override;
	virtual const double GetScale() const override;private:
	void Reset();
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
	int m_oldX;
	int m_oldY;
	IInput * m_input;
};