#include "Object.h"
#include "MovementLimiter.h"
#include <algorithm>
#define _USE_MATH_DEFINES
#include <float.h>
#include <math.h>

CObject::CObject(const Path& model, CVector3f const& position, float rotation, bool hasShadow)
	: BaseObject(model, position, rotation, hasShadow)
	, m_isSelectable(true)
{
}

std::set<std::string> const& CObject::GetHiddenMeshes() const
{
	return m_hiddenMeshes;
}

void CObject::ShowMesh(std::string const& meshName)
{
	auto i = m_hiddenMeshes.find(meshName);
	if (i != m_hiddenMeshes.end())
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
	if (m_properties.find(key) != m_properties.end())
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

void CObject::SetMovementLimiter(IMoveLimiter* limiter)
{
	m_movelimiter.reset(limiter);
}

std::map<std::wstring, std::wstring> const& CObject::GetAllProperties() const
{
	return m_properties;
}

void CObject::PlayAnimation(std::string const& animation, AnimationLoop loop, float speed)
{
	m_animation = animation;
	m_animationLoop = loop;
	m_animationSpeed = speed;
	m_animationTime = std::chrono::microseconds(0ll);
}

std::string CObject::GetAnimation() const
{
	return m_animation;
}

float CObject::GetAnimationTime() const
{
	return std::chrono::duration<float>(m_animationTime).count();
}

void CObject::AddSecondaryModel(const Path& model)
{
	m_secondaryModels.push_back(model);
}

void CObject::RemoveSecondaryModel(const Path& model)
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

Path CObject::GetSecondaryModel(size_t index) const
{
	return m_secondaryModels[index];
}

AnimationLoop CObject::GetAnimationLoop() const
{
	return m_animationLoop;
}

float CObject::GetAnimationSpeed() const
{
	return m_animationSpeed;
}

void CObject::Update(std::chrono::microseconds timeSinceLastUpdate)
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
	auto it = std::find_if(m_teamColor.begin(), m_teamColor.end(), [&suffix](sTeamColor const& tc) { return tc.suffix == suffix; });
	if (it != m_teamColor.end())
	{
		m_teamColor.erase(it);
	}
	m_teamColor.push_back(tc);
}

void CObject::ReplaceTexture(const Path& oldTexture, const Path& newTexture)
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

std::map<Path, Path> const& CObject::GetReplaceTextures() const
{
	return m_replaceTextures;
}

bool CObject::IsGroup() const
{
	return false;
}

IObject* CObject::GetFullObject()
{
	return this;
}
