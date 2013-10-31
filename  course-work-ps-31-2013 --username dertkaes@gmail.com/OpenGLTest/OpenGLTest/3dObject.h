#pragma once
#include "ObjectInterface.h"
#include <string>

class C3DObject : public IObject
{
public:
	C3DObject(std::string const& model, double x, double y, double rotation):m_model(model), m_x(x), m_y(y), m_z(0), m_rotation(rotation)  {}
	std::string GetPathToModel() const { return m_model; }
	void MoveTo(double x, double y, double rotation) { m_x = x; m_y = y; m_rotation = rotation; }
	double GetX() const { return m_x; }
	double GetY() const { return m_y; }
	double GetZ() const { return m_z; }
	double GetRotation() const { return m_rotation; }
private:
	std::string m_model;
	double m_x;
	double m_y;
	double m_z;
	double m_rotation;
};