#pragma once
#include "ICamera.h"

class CCameraMirror : public ICamera
{
public:
	CCameraMirror(ICamera * parentCamera, CVector3d const& translation = CVector3d());
	virtual CVector3d GetPosition() const override;
	virtual CVector3d GetDirection() const override;
	virtual CVector3d GetUpVector() const override;
	virtual const double GetScale() const override;
	virtual void SetInput(IInput & input) override;
private:
	ICamera * m_parentCamera;
	CVector3d m_translation;
};
