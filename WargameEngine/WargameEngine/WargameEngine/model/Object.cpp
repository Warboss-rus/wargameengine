#include "Object.h"
#include <algorithm>
#define _USE_MATH_DEFINES
#include <float.h>
#include <math.h>

namespace wargameEngine
{
namespace model
{

Object::Object(const Path& model, CVector3f const& position, float rotation, bool hasShadow)
	: BaseObject(model, position, rotation, hasShadow)
	, m_isSelectable(true)
{
}

std::set<std::string> const& Object::GetHiddenMeshes() const
{
	return m_hiddenMeshes;
}

void Object::ShowMesh(std::string const& meshName)
{
	auto i = m_hiddenMeshes.find(meshName);
	if (i != m_hiddenMeshes.end())
	{
		m_hiddenMeshes.erase(i);
	}
}

void Object::HideMesh(std::string const& meshName)
{
	m_hiddenMeshes.insert(meshName);
}

void Object::SetProperty(std::wstring const& key, std::wstring const& value)
{
	m_properties[key] = value;
}

std::wstring const Object::GetProperty(std::wstring const& key) const
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

bool Object::IsSelectable() const
{
	return m_isSelectable;
}

void Object::SetSelectable(bool selectable)
{
	m_isSelectable = selectable;
}

std::unordered_map<std::wstring, std::wstring> const& Object::GetAllProperties() const
{
	return m_properties;
}

void Object::PlayAnimation(std::string const& animation, AnimationLoop loop, float speed)
{
	m_animation = animation;
	m_animationLoop = loop;
	m_animationSpeed = speed;
	m_animationTime = std::chrono::microseconds(0ll);
}

std::string Object::GetAnimation() const
{
	return m_animation;
}

float Object::GetAnimationTime() const
{
	return std::chrono::duration<float>(m_animationTime).count();
}

void Object::AddSecondaryModel(const Path& model)
{
	m_secondaryModels.push_back(model);
}

void Object::RemoveSecondaryModel(const Path& model)
{
	for (auto i = m_secondaryModels.begin(); i != m_secondaryModels.end(); ++i)
	{
		if (*i == model)
		{
			m_secondaryModels.erase(i);
		}
	}
}

size_t Object::GetSecondaryModelsCount() const
{
	return m_secondaryModels.size();
}

Path Object::GetSecondaryModel(size_t index) const
{
	return m_secondaryModels[index];
}

AnimationLoop Object::GetAnimationLoop() const
{
	return m_animationLoop;
}

float Object::GetAnimationSpeed() const
{
	return m_animationSpeed;
}

void Object::Update(std::chrono::microseconds timeSinceLastUpdate)
{
	m_animationTime += timeSinceLastUpdate;
}

std::vector<TeamColor> const& Object::GetTeamColor() const
{
	return m_teamColor;
}

void Object::ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b)
{
	TeamColor tc;
	tc.suffix = suffix;
	tc.color[0] = r;
	tc.color[1] = g;
	tc.color[2] = b;
	auto it = std::find_if(m_teamColor.begin(), m_teamColor.end(), [&suffix](TeamColor const& tc) { return tc.suffix == suffix; });
	if (it != m_teamColor.end())
	{
		m_teamColor.erase(it);
	}
	m_teamColor.push_back(tc);
}

void Object::ReplaceTexture(const Path& oldTexture, const Path& newTexture)
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

std::unordered_map<Path, Path> const& Object::GetReplaceTextures() const
{
	return m_replaceTextures;
}

bool Object::IsGroup() const
{
	return false;
}

IObject* Object::GetFullObject()
{
	return this;
}

}
}