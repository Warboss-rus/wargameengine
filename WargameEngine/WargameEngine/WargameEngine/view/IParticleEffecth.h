#pragma once
#include <string>
#include "Vector3.h"

class IParticleEffect
{
public:
	virtual std::string const& GetModel() const = 0;
	virtual CVector3d const& GetCoords() = 0;
	virtual double GetRotation() const = 0;
	virtual double GetScale() const = 0;
	virtual float GetTime() const = 0;
	virtual bool IsEnded() const = 0;
};