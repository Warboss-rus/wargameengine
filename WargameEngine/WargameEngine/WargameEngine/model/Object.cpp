#include "Object.h"
#include <sys/timeb.h> 
#include <algorithm>
#include "MovementLimiter.h"
#define _USE_MATH_DEFINES
#include <math.h>

CObject::CObject(std::string const& model, double x, double y, double z, double rotation, bool hasShadow)
	:m_model(model), m_coords(x, y, z), m_rotation(rotation), m_isSelectable(true), m_castsShadow(hasShadow), m_animationBegin(0L), m_goSpeed(0.0f)
{
	struct timeb time;
	ftime(&time);
	m_lastUpdateTime = 1000 * time.time + time.millitm;
}

std::string CObject::GetPathToModel() const
{
	return m_model;
}

void CObject::Move(double x, double y, double z)
{ 
	m_coords.x += x; 
	m_coords.y += y;
	m_coords.z += z;
	if (m_movelimiter) m_movelimiter->FixPosition(m_coords, m_rotation, m_coords, m_rotation);
}

void CObject::SetCoords(double x, double y, double z) 
{ 
	m_coords = CVector3d(x, y, z);
	if (m_movelimiter) m_movelimiter->FixPosition(m_coords, m_rotation, m_coords, m_rotation);
}

void CObject::SetCoords(CVector3d const& coords)
{
	m_coords = coords;
}

void CObject::Rotate(double rotation)
{ 
	m_rotation = fmod(m_rotation + rotation + 360.0, 360); 
	if (m_movelimiter) m_movelimiter->FixPosition(m_coords, m_rotation, m_coords, m_rotation);
}

double CObject::GetX() const
{
	return m_coords.x;
}

double CObject::GetY() const
{
	return m_coords.y;
}

double CObject::GetZ() const
{
	return m_coords.z;
}

CVector3d CObject::GetCoords() const
{
	return m_coords;
}

double CObject::GetRotation() const
{
	return m_rotation;
}

std::set<std::string> const& CObject::GetHiddenMeshes() const
{
	return m_hiddenMeshes;
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

bool CObject::IsSelectable() const
{
	return m_isSelectable;
}

void CObject::SetSelectable(bool selectable)
{
	m_isSelectable = selectable;
}

void CObject::SetMovementLimiter(IMoveLimiter * limiter)
{
	m_movelimiter.reset(limiter);
}

std::map<std::string, std::string> const& CObject::GetAllProperties() const
{
	return m_properties;
}

bool CObject::CastsShadow() const
{
	return m_castsShadow;
}

void CObject::PlayAnimation(std::string const& animation, eAnimationLoopMode loop, float speed)
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

void CObject::AddSecondaryModel(std::string const& model)
{
	m_secondaryModels.push_back(model);
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

size_t CObject::GetSecondaryModelsCount() const 
{
	return m_secondaryModels.size();
}

std::string CObject::GetSecondaryModel(size_t index) const
{
	return m_secondaryModels[index];
}

eAnimationLoopMode CObject::GetAnimationLoop() const
{
	return m_animationLoop;
}

float CObject::GetAnimationSpeed() const
{
	return m_animationSpeed;
}

void CObject::GoTo(CVector3d const& coords, double speed, std::string const& animation, float animationSpeed)
{
	m_goTarget = coords;
	m_goSpeed = speed;
	PlayAnimation(animation, eAnimationLoopMode::LOOPING, animationSpeed);
}

void CObject::Update()
{
	struct timeb time;
	ftime(&time);
	long long current = 1000 * time.time + time.millitm;
	if (abs(m_goSpeed) < DBL_EPSILON)
	{
		m_lastUpdateTime = current;
		return;
	}
	CVector3d dir = m_goTarget - m_coords;
	dir.Normalize();
	m_rotation = atan2(dir.y, dir.x) * 180 / M_PI;
	dir = dir * static_cast<double>(current - m_lastUpdateTime) / 1000.0 * m_goSpeed;
	if (dir.GetLength() > (m_goTarget - m_coords).GetLength()) dir = (m_goTarget - m_coords);
	m_coords += dir;
	m_lastUpdateTime = current;
	if ((m_coords - m_goTarget).GetLength() < 0.0001)
	{
		m_goSpeed = 0.0;
		PlayAnimation("", eAnimationLoopMode::NONLOOPING, 0.0f);
	}
}

std::vector<sTeamColor> const& CObject::GetTeamColor() const
{
	return m_teamColor;
}

void CObject::ApplyTeamColor(std::string const& suffix, unsigned char r, unsigned char g, unsigned char b)
{
	sTeamColor tc;
	tc.suffix = suffix;
	tc.color[0] = r;
	tc.color[1] = g;
	tc.color[2] = b;
	auto it = std::find_if(m_teamColor.begin(), m_teamColor.end(), [&suffix](sTeamColor const& tc) {return tc.suffix == suffix;});
	if (it != m_teamColor.end())
	{
		m_teamColor.erase(it);
	}
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

std::map<std::string, std::string> const& CObject::GetReplaceTextures() const
{
	return m_replaceTextures;
}
