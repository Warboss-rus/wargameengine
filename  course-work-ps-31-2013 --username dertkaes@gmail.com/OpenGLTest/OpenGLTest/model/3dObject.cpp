#include "3dObject.h"

C3DObject::C3DObject(std::string const& model, double x, double y, double rotation)
	:m_model(model), m_x(x), m_y(y), m_z(0), m_rotation(rotation)  {}

void C3DObject::Move(double x, double y, double z) 
{ 
	m_x += x; 
	m_y += y; 
	m_z += z; 
	if(m_movelimiter) m_movelimiter->FixPosition(m_x, m_y, m_z, m_rotation);
}

void C3DObject::SetCoords(double x, double y, double z) 
{ 
	m_x = x; 
	m_y = y; 
	m_z = z;
	if(m_movelimiter) m_movelimiter->FixPosition(m_x, m_y, m_z, m_rotation);
}

void C3DObject::Rotate(double rotation) 
{ 
	m_rotation = fmod(m_rotation + rotation + 360.0, 360); 
	if(m_movelimiter) m_movelimiter->FixPosition(m_x, m_y, m_z, m_rotation);
}

void C3DObject::ShowMesh(std::string const& meshName) 
{ 
	auto i = m_hiddenMeshes.find(meshName);
	if(i != m_hiddenMeshes.end())
	{
		m_hiddenMeshes.erase(i);
	}
}

void C3DObject::HideMesh(std::string const& meshName) 
{ 
	m_hiddenMeshes.insert(meshName); 
}

void C3DObject::SetProperty(std::string const& key, std::string const& value) 
{ 
	m_properties[key] = value; 
}

std::string const C3DObject::GetProperty(std::string const& key) const 
{
	if(m_properties.find(key) != m_properties.end())
	{
		return m_properties.find(key)->second;
	}
	else
	{
		return "";	
	}
}