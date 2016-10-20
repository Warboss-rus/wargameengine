#pragma once
#include "IInput.h"
#include "Vector3.h"

class ICamera
{
public:
	virtual CVector3d GetPosition() const = 0;
	virtual CVector3d GetDirection() const = 0;
	virtual CVector3d GetUpVector() const = 0;
	virtual const double GetScale() const = 0;
	virtual void SetInput(IInput & input) = 0;
	virtual void EnableTouchMode() = 0;
	virtual ~ICamera() {}
};