#pragma once
#include "IInput.h"

class ICamera
{
public:
	virtual const double * GetPosition() const = 0;
	virtual const double * GetDirection() const = 0;
	virtual const double * GetUpVector() const = 0;
	virtual const double GetScale() const = 0;
	virtual void SetInput(IInput & input) = 0;
	virtual ~ICamera() {}
};