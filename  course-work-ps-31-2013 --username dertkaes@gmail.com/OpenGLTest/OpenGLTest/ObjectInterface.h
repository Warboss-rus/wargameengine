#pragma once

class IObject
{
public:
	virtual void Draw() const = 0;
	virtual ~IObject() {}
	virtual void Move(double deltaX, double deltaY, double deltaRotate) = 0;
	virtual double GetX() const = 0;
	virtual double GetY() const = 0;
	virtual double GetZ() const = 0;
	virtual double GetRotation() const = 0;
	virtual const double* GetBounding() const = 0;
};