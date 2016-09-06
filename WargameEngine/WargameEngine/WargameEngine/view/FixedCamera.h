#pragma once
#include "ICamera.h"

class CFixedCamera : public ICamera
{
public:
	CFixedCamera(CVector3d const& position, CVector3d const& direction, CVector3d const& up, double scale = 1.0);
	virtual CVector3d GetPosition() const override;
	virtual CVector3d GetDirection() const override;
	virtual CVector3d GetUpVector() const override;
	virtual const double GetScale() const override;
	virtual void SetInput(IInput & input) override;
private:
	CVector3d m_position;
	CVector3d m_direction;
	CVector3d m_up;
	double m_scale;
};
