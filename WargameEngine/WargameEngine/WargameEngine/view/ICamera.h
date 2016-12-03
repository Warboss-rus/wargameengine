#pragma once
#include "IInput.h"
#include "Vector3.h"

class ICamera
{
public:
	virtual CVector3f GetPosition() const = 0;
	virtual CVector3f GetDirection() const = 0;
	virtual CVector3f GetUpVector() const = 0;
	virtual const float GetScale() const = 0;
	virtual void SetInput(IInput & input) = 0;
	virtual void EnableTouchMode() = 0;
	virtual ~ICamera() {}
};