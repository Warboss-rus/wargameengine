#include "Object.h"
#include <algorithm>
#include "MovementLimiter.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

CObject::CObject(std::wstring const& model, CVector3f const& position, float rotation, bool hasShadow)
	: CBaseObject(model, position, rotation, hasShadow), m_isSelectable(true), m_animationTime(0L)
{
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

void CObject::SetProperty(std::wstring const& key, std::wstring const& value) 
{ 
	m_properties[key] = value; 
}

std::wstring const CObject::GetProperty(std::wstring const& key) const 
{
	if(m_properties.find(key) != m_properties.end())
	{
		return m_properties.find(key)->second;
	}
	else
	{
		return L"";	
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

std::map<std::wstring, std::wstring> const& CObject::GetAllProperties() const
{
	return m_properties;
}

void CObject::PlayAnimation(std::string const& animation, eAnimationLoopMode loop, float speed)
{
	m_animation = animation;
	m_animationLoop = loop;
	m_animationSpeed = speed;
	m_animationTime = 0;
}

std::string CObject::GetAnimation() const
{
	return m_animation;
}

float CObject::GetAnimationTime() const
{
	if (m_animationTime == 0L) return 0.0f;
	return static_cast<float>((double)m_animationTime / 1000.0);
}

void CObject::AddSecondaryModel(std::wstring const& model)
{
	m_secondaryModels.push_back(model);
}

void CObject::RemoveSecondaryModel(std::wstring const& model)
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

std::wstring CObject::GetSecondaryModel(size_t index) const
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

void CObject::Update(long long timeSinceLastUpdate)
{
	m_animationTime += timeSinceLastUpdate;
}

std::vector<sTeamColor> const& CObject::GetTeamColor() const
{
	return m_teamColor;
}

void CObject::ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b)
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

void CObject::ReplaceTexture(std::wstring const& oldTexture, std::wstring const& newTexture)
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

std::map<std::wstring, std::wstring> const& CObject::GetReplaceTextures() const
{
	return m_replaceTextures;
}

bool CObject::IsGroup() const
{
	return false;
}
