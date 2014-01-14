#pragma once
#include <string>
#include <set>
#include "..\view\Vector3.h"
#include "MovementLimiter.h"
#include "../lib/src/Math/float3.h"

class IObject
{
public:
	virtual std::string GetPathToModel() const = 0;
	virtual ~IObject() {}
	virtual void Move(float x, float y, float z) = 0;
	virtual void SetCoords(float x, float y, float z) = 0;
	virtual void SetCoords(float3 pos) = 0;
	virtual void Rotate(float rotation) = 0;
	virtual float GetX() const = 0;
	virtual float GetY() const = 0;
	virtual float GetZ() const = 0;
	virtual CVector3d GetCoords() const = 0;
	virtual float GetRotation() const = 0;
	virtual std::set<std::string> const& GetHiddenMeshes() const = 0;
	virtual void HideMesh(std::string const& meshName) = 0;
	virtual void ShowMesh(std::string const& meshName) = 0;
	virtual void SetProperty(std::string const& key, std::string const& value) = 0;
	virtual std::string const GetProperty(std::string const& key) const = 0;
	virtual bool IsSelectable() const = 0;
	virtual void SetSelectable(bool selectable) = 0;
	virtual void SetMovementLimiter(IMoveLimiter * limiter) = 0;
};