#pragma once
#include "../Signal.h"
#include "../Typedefs.h"
#include "../view/Vector3.h"

namespace wargameEngine
{
namespace model
{
class IObject;

class IBaseObject
{
public:
	//CVector3f oldPosition, CVector3f newPosition
	typedef signals::Signal<void, const CVector3f&, const CVector3f&> CoordsSignal;
	//CVector3f oldRotations, CVector3f newRotations
	typedef signals::Signal<void, const CVector3f&, const CVector3f&> RotationSignal;

	virtual ~IBaseObject() {}

	virtual Path GetPathToModel() const = 0;
	virtual void SetCoords(float x, float y, float z) = 0;
	virtual void SetCoords(CVector3f const& coords) = 0;
	virtual void Move(float dx, float dy, float dz) = 0;
	virtual CVector3f GetCoords() const = 0;
	virtual float GetX() const = 0;
	virtual float GetY() const = 0;
	virtual float GetZ() const = 0;
	virtual void Rotate(float rotation) = 0;
	virtual void SetRotation(float rotation) = 0;
	virtual float GetRotation() const = 0;
	virtual CVector3f GetRotations() const = 0;
	virtual void SetRotations(const CVector3f& rotations) = 0;
	virtual bool CastsShadow() const = 0;
	virtual IObject* GetFullObject() = 0;
	virtual signals::SignalConnection DoOnCoordsChange(CoordsSignal::Slot const& handler) = 0;
	virtual signals::SignalConnection DoOnRotationChange(RotationSignal::Slot const& handler) = 0;
};
}
}