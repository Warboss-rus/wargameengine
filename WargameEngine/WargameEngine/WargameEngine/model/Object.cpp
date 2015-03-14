#include "Object.h"
#include <sys/timeb.h> 

CObject::CObject(std::string const& model, double x, double y, double rotation, bool hasShadow)
	:m_model(model), m_coords(x, y, 0), m_rotation(rotation), m_isSelectable(true), m_castsShadow(hasShadow), m_animationBegin(0L), m_goSpeed(0.0f)
{
	struct timeb time;
	ftime(&time);
	m_lastUpdateTime = 1000 * time.time + time.millitm;
}

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

void CObject::PlayAnimation(std::string const& animation, sAnimation::eLoopMode loop, float speed)
{
	m_animation = animation;
	m_animationLoop = loop;
	m_animationSpeed = speed;
	struct timeb time;
	ftime(&time);
	m_animationBegin = 1000 * time.time + time.millitm;
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
	long long delta = 1000 * time.time + time.millitm - m_animationBegin;
	return static_cast<float>((double)delta / 1000.0);
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

void CObject::GoTo(CVector3d const& coords, double speed, std::string const& animation, float animationSpeed)
{
	m_goTarget = coords;
	m_goSpeed = speed;
	PlayAnimation(animation, sAnimation::LOOPING, animationSpeed);
}

void CObject::Update()
{
	struct timeb time;
	ftime(&time);
	long long current = 1000 * time.time + time.millitm;
	if (m_goSpeed == 0.0)
	{
		m_lastUpdateTime = current;
		return;
	}
	CVector3d dir = m_goTarget - m_coords;
	dir.Normalize();
	m_rotation = atan2(dir.y, dir.x) * 180 / 3.1415;
	dir = dir * static_cast<double>(current - m_lastUpdateTime) / 1000.0 * m_goSpeed;
	if (dir.GetLength() > (m_goTarget - m_coords).GetLength()) dir = (m_goTarget - m_coords);
	m_coords += dir;
	m_lastUpdateTime = current;
	if ((m_coords - m_goTarget).GetLength() < 0.0001)
	{
		m_goSpeed = 0.0;
		PlayAnimation("", sAnimation::NONLOOPING, 0.0f);
	}
}

void CObject::ApplyTeamColor(std::string const& suffix, unsigned char r, unsigned char g, unsigned char b)
{
	sTeamColor tc;
	tc.suffix = suffix;
	tc.color[0] = r;
	tc.color[1] = g;
	tc.color[2] = b;
	m_teamColor.push_back(tc);
}

void CObject::ReplaceTexture(std::string const& oldTexture, std::string const& newTexture)
{
	if (newTexture.empty())
	{
		m_replaceTextures.erase(oldTexture);
	}
	else
	{
		m_replaceTextures[oldTexture] = newTexture;
	}
}