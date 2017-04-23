#include "ObjectGroup.h"
#include "IGameModel.h"
#include <algorithm>
#include "../Utils.h"

CObjectGroup::CObjectGroup(IGameModel & model) :m_current(0), m_model(model)
{

}

Path CObjectGroup::GetPathToModel() const
{
	if(m_children.empty()) return make_path(L"");
	return m_children[m_current]->GetPathToModel();
}

void CObjectGroup::Move(float dx, float dy, float dz)
{
	for(auto& child : m_children)
	{
		child->Move(dx, dy, dz);
	}
}

void CObjectGroup::SetCoords(float x, float y, float z)
{
	x -= GetX();
	y -= GetY();
	z -= GetZ();
	for(auto& child : m_children)
	{
		child->Move(x, y, z);
	}
}

void CObjectGroup::SetCoords(CVector3f const& coords)
{
	float x = coords.x - GetX();
	float y = coords.y - GetY();
	float z = coords.z - GetZ();
	for (auto& child : m_children)
	{
		child->Move(x, y, z);
	}
}

void CObjectGroup::Rotate(float rotation)
{
	for(auto& child : m_children)
	{
		child->Rotate(rotation);
	}
}

void CObjectGroup::SetRotation(float rotation)
{
	for (auto& child : m_children)
	{
		child->SetRotation(rotation);
	}
}

float CObjectGroup::GetX() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetX();
	}
	return 0.0f;
}

float CObjectGroup::GetY() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetY();
	}
	return 0.0f;
}

float CObjectGroup::GetZ() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetZ();
	}
	return 0.0f;
}

CVector3f CObjectGroup::GetCoords() const
{
	return CVector3f(GetX(), GetY(), GetZ());
}

float CObjectGroup::GetRotation() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetRotation();
	}
	return 0.0f;
}

std::set<std::string> const& CObjectGroup::GetHiddenMeshes() const
{
	return empty;
}

void CObjectGroup::HideMesh(std::string const& meshName)
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->HideMesh(meshName);
	}
}

void CObjectGroup::ShowMesh(std::string const& meshName)
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->ShowMesh(meshName);
	}
}

void CObjectGroup::AddChildren(std::shared_ptr<IObject> const& object)
{
	if(object)
	{
		m_children.push_back(object);
	}
}

void CObjectGroup::RemoveChildren(std::shared_ptr<IObject> const& object)
{
	if(object == m_children[m_current]) m_current = 0;
	auto it = std::find(m_children.begin(), m_children.end(), object);
	m_children.erase(it, m_children.end());
}

bool CObjectGroup::ContainsChildren(std::shared_ptr<IObject> const& object) const
{
	auto it = std::find(m_children.begin(), m_children.end(), object);
	return it != m_children.end();
}

size_t CObjectGroup::GetCount() const
{
	return m_children.size();
}

std::shared_ptr<IObject> CObjectGroup::GetChild(size_t index)
{
	return m_children[index];
}

void CObjectGroup::DeleteAll()
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		m_model.DeleteObjectByPtr(*i);
	}
	m_children.clear();
}

void CObjectGroup::SetCurrent(std::shared_ptr<IObject> const& object)
{
	auto it = std::find(m_children.begin(), m_children.end(), object);
	if (it != m_children.end())
	{
		m_current = it - m_children.begin();
	}
}

std::shared_ptr<IObject> CObjectGroup::GetCurrent() const
{
	return m_children[m_current];
}

void CObjectGroup::SetProperty(std::wstring const& key, std::wstring const& value)
{ 
	for(auto& child : m_children)
	{
		child->SetProperty(key, value);
	}
}
std::wstring const CObjectGroup::GetProperty(std::wstring const& key) const 
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetProperty(key);
	}
	return L"";
}

bool CObjectGroup::IsSelectable() const
{
	return true;
}

void CObjectGroup::SetSelectable(bool selectable)
{
	for(auto& child : m_children)
	{
		child->SetSelectable(selectable);
	}
}

void CObjectGroup::SetMovementLimiter(IMoveLimiter * limiter)
{
	for(auto& child : m_children)
	{
		child->SetMovementLimiter(limiter);
	}
}

std::map<std::wstring, std::wstring> const& CObjectGroup::GetAllProperties() const
{
	return m_children[0]->GetAllProperties();
}

bool CObjectGroup::CastsShadow() const
{
	if (m_children.empty()) return true;
	return m_children[m_current]->CastsShadow();
}

void CObjectGroup::PlayAnimation(std::string const& animation, eAnimationLoopMode loop, float speed)
{
	for (auto& child : m_children)
	{
		child->PlayAnimation(animation, loop, speed);
	}
}

std::string CObjectGroup::GetAnimation() const
{
	if (m_children.empty()) return "";
	return m_children[m_current]->GetAnimation();
}

float CObjectGroup::GetAnimationTime() const
{
	if (m_children.empty()) return true;
	return m_children[m_current]->GetAnimationTime();
}

void CObjectGroup::AddSecondaryModel(const Path& model)
{
	for (auto& child : m_children)
	{
		child->AddSecondaryModel(model);
	}
}

void CObjectGroup::RemoveSecondaryModel(const Path& model)
{
	for (auto& child : m_children)
	{
		child->RemoveSecondaryModel(model);
	}
}

size_t CObjectGroup::GetSecondaryModelsCount() const
{
	if (m_children.empty()) return 0;
	return m_children[m_current]->GetSecondaryModelsCount();
}

Path CObjectGroup::GetSecondaryModel(size_t index) const
{
	if (m_children.empty()) return Path();
	return m_children[m_current]->GetSecondaryModel(index);
}

eAnimationLoopMode CObjectGroup::GetAnimationLoop() const
{
	if (m_children.empty()) return eAnimationLoopMode::NONLOOPING;
	return m_children[m_current]->GetAnimationLoop();
}

float CObjectGroup::GetAnimationSpeed() const
{
	if (m_children.empty()) return 1.0f;
	return m_children[m_current]->GetAnimationSpeed();
}

void CObjectGroup::Update(std::chrono::microseconds timeSinceLastUpdate)
{
	for (auto& child : m_children)
	{
		child->Update(timeSinceLastUpdate);
	}
}

void CObjectGroup::ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b)
{
	for (auto& child : m_children)
	{
		child->ApplyTeamColor(suffix, r, g, b);
	}
}

void CObjectGroup::ReplaceTexture(const Path& oldTexture, const Path& newTexture)
{
	for (auto& child : m_children)
	{
		child->ReplaceTexture(oldTexture, newTexture);
	}
}

std::vector<sTeamColor> const& CObjectGroup::GetTeamColor() const
{ 
	if(m_children.empty())
	{
		static std::vector<sTeamColor> dummy;
		return dummy;
	}
	return m_children[m_current]->GetTeamColor();
}

std::map<Path, Path> const& CObjectGroup::GetReplaceTextures() const
{ 
	if (m_children.empty())
	{
		static std::map<Path, Path> dummy;
		return dummy;
	}
	return m_children[m_current]->GetReplaceTextures();
}

bool CObjectGroup::IsGroup() const
{
	return true;
}

IObject* CObjectGroup::GetFullObject()
{
	return nullptr;
}

CSignalConnection CObjectGroup::DoOnCoordsChange(CoordsSignal::Slot const& handler)
{
	return m_children[m_current]->DoOnCoordsChange(handler);
}

CSignalConnection CObjectGroup::DoOnRotationChange(RotationSignal::Slot const& handler)
{
	return m_children[m_current]->DoOnRotationChange(handler);
}
