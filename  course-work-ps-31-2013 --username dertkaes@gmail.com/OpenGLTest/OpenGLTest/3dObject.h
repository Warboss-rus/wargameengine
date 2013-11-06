#pragma once
#include "ObjectInterface.h"
#include <string>
#include <map>

class C3DObject : public IObject
{
public:
	C3DObject(std::string const& model, double x, double y, double rotation);
	std::string GetPathToModel() const { return m_model; }
	void Move(double x, double y, double z);
	void SetCoords(double x, double y, double z);
	void Rotate(double rotation);
	double GetX() const { return m_x; }
	double GetY() const { return m_y; }
	double GetZ() const { return m_z; }
	sPoint3 GetCoords() const { return sPoint3(m_x, m_y, m_z); }
	double GetRotation() const { return m_rotation; }
	std::set<std::string> const& GetHiddenMeshes() const { return m_hiddenMeshes; }
	void HideMesh(std::string const& meshName);
	void ShowMesh(std::string const& meshName);
	void SetProperty(std::string const& key, std::string const& value);
	std::string const GetProperty(std::string const& key) const;
	bool IsSelectable() const { return m_isSelectable; }
	void SetSelectable(bool selectable) { m_isSelectable = selectable; }
	void SetRelocatable(bool relocatable) { m_relocatable = relocatable; }
private:
	std::string m_model;
	double m_x;
	double m_y;
	double m_z;
	double m_rotation;
	std::set<std::string> m_hiddenMeshes;
	std::map<std::string, std::string> m_properties;
	bool m_isSelectable;
	bool m_relocatable;
};