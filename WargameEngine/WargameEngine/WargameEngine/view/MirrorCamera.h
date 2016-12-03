#pragma once
#include "ICamera.h"

class CCameraMirror : public ICamera
{
public:
	CCameraMirror(ICamera * parentCamera, CVector3f const& translation = CVector3f());
	virtual CVector3f GetPosition() const override;
	virtual CVector3f GetDirection() const override;
	virtual CVector3f GetUpVector() const override;
	virtual const float GetScale() const override;
	virtual void SetInput(IInput & input) override;
	virtual void EnableTouchMode() override;
private:
	ICamera * m_parentCamera;
	CVector3f m_translation;
};
