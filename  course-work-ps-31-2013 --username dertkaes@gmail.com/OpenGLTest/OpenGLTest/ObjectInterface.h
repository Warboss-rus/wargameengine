#pragma once
#include <string>
#include <set>

class IObject
{
public:
	virtual std::string GetPathToModel() const = 0;
	virtual ~IObject() {}
	virtual void Move(double x, double y, double z) = 0;
	virtual void Rotate(double rotation) = 0;
	virtual double GetX() const = 0;
	virtual double GetY() const = 0;
	virtual double GetZ() const = 0;
	virtual double GetRotation() const = 0;
	virtual std::set<std::string> const& GetHiddenMeshes() const = 0;
	virtual void HideMesh(std::string const& meshName) = 0;
	virtual void ShowMesh(std::string const& meshName) = 0;
};