#include "ObjectGroup.h"
#include "IModel.h"
#include <algorithm>
#include "../Utils.h"

namespace wargameEngine
{
namespace model
{

ObjectGroup::ObjectGroup(IModel & model) :m_current(0), m_model(model)
{

}

Path ObjectGroup::GetPathToModel() const
{
	if (m_children.empty()) return make_path(L"");
	return m_children[m_current]->GetPathToModel();
}

void ObjectGroup::Move(float dx, float dy, float dz)
{
	for (auto& child : m_children)
	{
		child->Move(dx, dy, dz);
	}
}

void ObjectGroup::SetCoords(float x, float y, float z)
{
	x -= GetX();
	y -= GetY();
	z -= GetZ();
	for (auto& child : m_children)
	{
		child->Move(x, y, z);
	}
}

void ObjectGroup::SetCoords(CVector3f const& coords)
{
	float x = coords.x - GetX();
	float y = coords.y - GetY();
	float z = coords.z - GetZ();
	for (auto& child : m_children)
	{
		child->Move(x, y, z);
	}
}

void ObjectGroup::Rotate(float rotation)
{
	for (auto& child : m_children)
	{
		child->Rotate(rotation);
	}
}

void ObjectGroup::SetRotation(float rotation)
{
	for (auto& child : m_children)
	{
		child->SetRotation(rotation);
	}
}

void ObjectGroup::SetRotations(const CVector3f& rotations)
{
	for (auto& child : m_children)
	{
		child->SetRotations(rotations);
	}
}

CVector3f ObjectGroup::GetRotations() const
{
	if (m_current < m_children.size())
	{
		return m_children[m_current]->GetRotations();
	}
	return CVector3f();
}

float ObjectGroup::GetX() const
{
	if (m_current < m_children.size())
	{
		return m_children[m_current]->GetX();
	}
	return 0.0f;
}

float ObjectGroup::GetY() const
{
	if (m_current < m_children.size())
	{
		return m_children[m_current]->GetY();
	}
	return 0.0f;
}

float ObjectGroup::GetZ() const
{
	if (m_current < m_children.size())
	{
		return m_children[m_current]->GetZ();
	}
	return 0.0f;
}

CVector3f ObjectGroup::GetCoords() const
{
	return CVector3f(GetX(), GetY(), GetZ());
}

float ObjectGroup::GetRotation() const
{
	if (m_current < m_children.size())
	{
		return m_children[m_current]->GetRotation();
	}
	return 0.0f;
}

std::set<std::string> const& ObjectGroup::GetHiddenMeshes() const
{
	return empty;
}

void ObjectGroup::HideMesh(std::string const& meshName)
{
	for (auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->HideMesh(meshName);
	}
}

void ObjectGroup::ShowMesh(std::string const& meshName)
{
	for (auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->ShowMesh(meshName);
	}
}

void ObjectGroup::AddChildren(std::shared_ptr<IObject> const& object)
{
	if (object)
	{
		m_children.push_back(object);
	}
}

void ObjectGroup::RemoveChildren(std::shared_ptr<IObject> const& object)
{
	if (object == m_children[m_current]) m_current = 0;
	auto it = std::find(m_children.begin(), m_children.end(), object);
	m_children.erase(it, m_children.end());
}

bool ObjectGroup::ContainsChildren(std::shared_ptr<IObject> const& object) const
{
	auto it = std::find(m_children.begin(), m_children.end(), object);
	return it != m_children.end();
}

size_t ObjectGroup::GetCount() const
{
	return m_children.size();
}

std::shared_ptr<IObject> ObjectGroup::GetChild(size_t index)
{
	return m_children[index];
}

void ObjectGroup::DeleteAll()
{
	for (auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		m_model.DeleteObjectByPtr(*i);
	}
	m_children.clear();
}

void ObjectGroup::SetCurrent(std::shared_ptr<IObject> const& object)
{
	auto it = std::find(m_children.begin(), m_children.end(), object);
	if (it != m_children.end())
	{
		m_current = it - m_children.begin();
	}
}

std::shared_ptr<IObject> ObjectGroup::GetCurrent() const
{
	return m_children[m_current];
}

void ObjectGroup::SetProperty(std::wstring const& key, std::wstring const& value)
{
	for (auto& child : m_children)
	{
		child->SetProperty(key, value);
	}
}
std::wstring const ObjectGroup::GetProperty(std::wstring const& key) const
{
	if (m_current < m_children.size())
	{
		return m_children[m_current]->GetProperty(key);
	}
	return L"";
}

bool ObjectGroup::IsSelectable() const
{
	return true;
}

void ObjectGroup::SetSelectable(bool selectable)
{
	for (auto& child : m_children)
	{
		child->SetSelectable(selectable);
	}
}

std::map<std::wstring, std::wstring> const& ObjectGroup::GetAllProperties() const
{
	return m_children[0]->GetAllProperties();
}

bool ObjectGroup::CastsShadow() const
{
	if (m_children.empty()) return true;
	return m_children[m_current]->CastsShadow();
}

void ObjectGroup::PlayAnimation(std::string const& animation, AnimationLoop loop, float speed)
{
	for (auto& child : m_children)
	{
		child->PlayAnimation(animation, loop, speed);
	}
}

std::string ObjectGroup::GetAnimation() const
{
	if (m_children.empty()) return "";
	return m_children[m_current]->GetAnimation();
}

float ObjectGroup::GetAnimationTime() const
{
	if (m_children.empty()) return true;
	return m_children[m_current]->GetAnimationTime();
}

void ObjectGroup::AddSecondaryModel(const Path& model)
{
	for (auto& child : m_children)
	{
		child->AddSecondaryModel(model);
	}
}

void ObjectGroup::RemoveSecondaryModel(const Path& model)
{
	for (auto& child : m_children)
	{
		child->RemoveSecondaryModel(model);
	}
}

size_t ObjectGroup::GetSecondaryModelsCount() const
{
	if (m_children.empty()) return 0;
	return m_children[m_current]->GetSecondaryModelsCount();
}

Path ObjectGroup::GetSecondaryModel(size_t index) const
{
	if (m_children.empty()) return Path();
	return m_children[m_current]->GetSecondaryModel(index);
}

AnimationLoop ObjectGroup::GetAnimationLoop() const
{
	if (m_children.empty()) return AnimationLoop::NonLooping;
	return m_children[m_current]->GetAnimationLoop();
}

float ObjectGroup::GetAnimationSpeed() const
{
	if (m_children.empty()) return 1.0f;
	return m_children[m_current]->GetAnimationSpeed();
}

void ObjectGroup::Update(std::chrono::microseconds timeSinceLastUpdate)
{
	for (auto& child : m_children)
	{
		child->Update(timeSinceLastUpdate);
	}
}

void ObjectGroup::ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b)
{
	for (auto& child : m_children)
	{
		child->ApplyTeamColor(suffix, r, g, b);
	}
}

void ObjectGroup::ReplaceTexture(const Path& oldTexture, const Path& newTexture)
{
	for (auto& child : m_children)
	{
		child->ReplaceTexture(oldTexture, newTexture);
	}
}

std::vector<TeamColor> const& ObjectGroup::GetTeamColor() const
{
	if (m_children.empty())
	{
		static std::vector<TeamColor> dummy;
		return dummy;
	}
	return m_children[m_current]->GetTeamColor();
}

std::map<Path, Path> const& ObjectGroup::GetReplaceTextures() const
{
	if (m_children.empty())
	{
		static std::map<Path, Path> dummy;
		return dummy;
	}
	return m_children[m_current]->GetReplaceTextures();
}

bool ObjectGroup::IsGroup() const
{
	return true;
}

IObject* ObjectGroup::GetFullObject()
{
	return nullptr;
}

signals::SignalConnection ObjectGroup::DoOnCoordsChange(CoordsSignal::Slot const& handler)
{
	return m_children[m_current]->DoOnCoordsChange(handler);
}

signals::SignalConnection ObjectGroup::DoOnRotationChange(RotationSignal::Slot const& handler)
{
	return m_children[m_current]->DoOnRotationChange(handler);
}

}
}