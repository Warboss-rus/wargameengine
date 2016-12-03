#pragma once
#include "ICamera.h"

class CFixedCamera : public ICamera
{
public:
	CFixedCamera(CVector3f const& position, CVector3f const& direction, CVector3f const& up, float scale = 1.0f);
	virtual CVector3f GetPosition() const override;
	virtual CVector3f GetDirection() const override;
	virtual CVector3f GetUpVector() const override;
	virtual const float GetScale() const override;
	virtual void SetInput(IInput & input) override;
	virtual void EnableTouchMode() override;
private:
	CVector3f m_position;
	CVector3f m_direction;
	CVector3f m_up;
	float m_scale;
};
