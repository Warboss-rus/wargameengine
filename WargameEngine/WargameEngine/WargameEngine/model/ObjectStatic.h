#pragma once
#include "ObjectInterface.h"
class CStaticObject
{
public:
	CStaticObject(std::string const& model, double x, double y, double rotation, bool hasShadow = true)
		:m_coords(x, y, 0.0), m_rotation(rotation), m_model(model), m_castsShadow(hasShadow) {}
	std::string GetPathToModel() const { return m_model; }
	double GetX() const { return m_coords.x; }
	double GetY() const { return m_coords.y; }
	double GetZ() const { return m_coords.z; }
	CVector3d GetCoords() const { return m_coords; }
	double GetRotation() const { return m_rotation; }
	bool CastsShadow() const { return m_castsShadow; }
protected:
	CVector3d m_coords;
	double m_rotation;
private:
	std::string m_model;
	bool m_castsShadow;
};