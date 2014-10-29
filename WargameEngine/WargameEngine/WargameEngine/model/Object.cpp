#include "Object.h"
#include <sys\timeb.h> 

CObject::CObject(std::string const& model, double x, double y, double rotation, bool hasShadow)
	:m_model(model), m_x(x), m_y(y), m_z(0), m_rotation(rotation), m_isSelectable(true), m_castsShadow(hasShadow), m_animationBegin(0L){}

void CObject::Move(double x, double y, double z) 
{ 
	m_x += x; 
	m_y += y; 
	m_z += z; 
	if(m_movelimiter) m_movelimiter->FixPosition(m_x, m_y, m_z, m_rotation);
}

void CObject::SetCoords(double x, double y, double z) 
{ 
	m_x = x; 
	m_y = y; 
	m_z = z;
	if(m_movelimiter) m_movelimiter->FixPosition(m_x, m_y, m_z, m_rotation);
}

void CObject::Rotate(double rotation) 
{ 
	m_rotation = fmod(m_rotation + rotation + 360.0, 360); 
	if(m_movelimiter) m_movelimiter->FixPosition(m_x, m_y, m_z, m_rotation);
}

void CObject::ShowMesh(std::string const& meshName) 
{ 
	auto i = m_hiddenMeshes.find(meshName);
	if(i != m_hiddenMeshes.end())
	{
		m_hiddenMeshes.erase(i);
	}
}

void CObject::HideMesh(std::string const& meshName) 
{ 
	m_hiddenMeshes.insert(meshName); 
}

void CObject::SetProperty(std::string const& key, std::string const& value) 
{ 
	m_properties[key] = value; 
}

std::string const CObject::GetProperty(std::string const& key) const 
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

void CObject::PlayAnimation(std::string const& animation)
{
	m_animation = animation;
	struct timeb time;
	ftime(&time);
	m_animationBegin = 1000 * time.time + time.millitm;
}

std::string CObject::GetAnimation() const
{
	return m_animation;
}

long CObject::GetAnimationTime() const
{
	if (m_animationBegin == 0L) return 0L;
	struct timeb time;
	ftime(&time);
	return 1000 * time.time + time.millitm - m_animationBegin;
}