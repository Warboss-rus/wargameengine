#pragma once
#include "ObjectInterface.h"
#include <string>
#include <map>

class C3DObject : public IObject
{
public:
	C3DObject(std::string const& model, double x, double y, double rotation):m_model(model), m_x(x), m_y(y), m_z(0), m_rotation(rotation)  {}
	std::string GetPathToModel() const { return m_model; }
	void Move(double x, double y, double z) { m_x += x; m_y += y; m_z += z; }
	void SetCoords(double x, double y, double z) { m_x = x; m_y = y; m_z = z; }
	void Rotate(double rotation) { m_rotation = fmod(m_rotation + rotation + 360.0, 360); }
	double GetX() const { return m_x; }
	double GetY() const { return m_y; }
	double GetZ() const { return m_z; }
	sPoint3 GetCoords() const { return sPoint3(m_x, m_y, m_z); }
	double GetRotation() const { return m_rotation; }
	std::set<std::string> const& GetHiddenMeshes() const { return m_hiddenMeshes; }
	void HideMesh(std::string const& meshName) { m_hiddenMeshes.insert(meshName); }
	void ShowMesh(std::string const& meshName) { 
		auto i = m_hiddenMeshes.find(meshName);
		if(i != m_hiddenMeshes.end())
			m_hiddenMeshes.erase(i);
	}
	void SetProperty(std::string const& key, std::string const& value) { m_properties[key] = value; }
	std::string const& GetProperty(std::string const& key) const {
		if(m_properties.find(key) != m_properties.end())
			return m_properties.find(key)->second;
		return "";	}
	bool IsSelectable() const { return m_isSelectable; }
	void SetSelectable(bool selectable) { m_isSelectable = selectable; }
private:
	std::string m_model;
	double m_x;
	double m_y;
	double m_z;
	double m_rotation;
	std::set<std::string> m_hiddenMeshes;
	std::map<std::string, std::string> m_properties;
	bool m_isSelectable;
};