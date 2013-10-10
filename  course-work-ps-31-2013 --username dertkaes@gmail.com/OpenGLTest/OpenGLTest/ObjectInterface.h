#pragma once
#include <string>

class IObject
{
public:
	virtual std::string GetPathToModel() const = 0;
	virtual ~IObject() {}
	virtual void MoveTo(double x, double y, double rotation) = 0;
	virtual double GetX() const = 0;
	virtual double GetY() const = 0;
	virtual double GetZ() const = 0;
	virtual double GetRotation() const = 0;
	virtual const double* GetBounding() const = 0;
};