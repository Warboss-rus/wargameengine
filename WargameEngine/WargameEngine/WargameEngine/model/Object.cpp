#include "Object.h"
#include <sys\timeb.h> 

CObject::CObject(std::string const& model, double x, double y, double rotation, bool hasShadow)
	:m_model(model), m_coords(x, y, 0), m_rotation(rotation), m_isSelectable(true), m_castsShadow(hasShadow), m_animationBegin(0L){}

void CObject::Move(double x, double y, double z) 
{ 
	m_coords.x += x; 
	m_coords.y += y;
	m_coords.z += z;
	if (m_movelimiter) m_movelimiter->FixPosition(m_coords, m_rotation);
}

void CObject::SetCoords(double x, double y, double z) 
{ 
	m_coords = CVector3d(x, y, z);
	if (m_movelimiter) m_movelimiter->FixPosition(m_coords, m_rotation);
}

void CObject::Rotate(double rotation) 
{ 
	m_rotation = fmod(m_rotation + rotation + 360.0, 360); 
	if (m_movelimiter) m_movelimiter->FixPosition(m_coords, m_rotation);
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

void CObject::PlayAnimation(std::string const& animation, sAnimation::eLoopMode loop)
{
	m_animation = animation;
	struct timeb time;
	ftime(&time);
	m_animationBegin = 1000 * time.time + time.millitm;
	m_animationLoop = loop;
}

std::string CObject::GetAnimation() const
{
	return m_animation;
}

float CObject::GetAnimationTime() const
{
	if (m_animationBegin == 0L) return 0.0f;
	struct timeb time;
	ftime(&time);
	long delta = 1000 * time.time + time.millitm - m_animationBegin;
	return (double)delta / 1000.0f;
}

void CObject::RemoveSecondaryModel(std::string const& model)
{
	for (auto i = m_secondaryModels.begin(); i != m_secondaryModels.end(); ++i)
	{
		if (*i == model)
		{
			m_secondaryModels.erase(i);
		}
	}
}