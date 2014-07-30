#pragma once
#include "ObjectInterface.h"
#include <string>
#include <map>
#include <memory>
#include "../view/Vector3.h"

class C3DObject : public IObject
{
public:
	C3DObject(std::string const& model, double x, double y, double rotation);
	std::string GetPathToModel() const { return m_model; }
	void Move(double x, double y, double z);
	void SetCoords(double x, double y, double z);
	void SetCoords(CVector3d const& coords) { m_x = coords.x; m_y = coords.y; m_z = coords.z; }
	void Rotate(double rotation);
	double GetX() const { return m_x; }
	double GetY() const { return m_y; }
	double GetZ() const { return m_z; }
	CVector3d GetCoords() const { return CVector3d(m_x, m_y, m_z); }
	double GetRotation() const { return m_rotation; }
	std::set<std::string> const& GetHiddenMeshes() const { return m_hiddenMeshes; }
	void HideMesh(std::string const& meshName);
	void ShowMesh(std::string const& meshName);
	void SetProperty(std::string const& key, std::string const& value);
	std::string const GetProperty(std::string const& key) const;
	bool IsSelectable() const { return m_isSelectable; }
	void SetSelectable(bool selectable) { m_isSelectable = selectable; }
	void SetMovementLimiter(IMoveLimiter * limiter) { m_movelimiter.reset(limiter); }
	std::map<std::string, std::string> const& GetAllProperties() const { return m_properties; }
private:
	std::string m_model;
	double m_x;
	double m_y;
	double m_z;
	double m_rotation;
	std::set<std::string> m_hiddenMeshes;
	std::map<std::string, std::string> m_properties;
	bool m_isSelectable;
	std::shared_ptr<IMoveLimiter> m_movelimiter;
};