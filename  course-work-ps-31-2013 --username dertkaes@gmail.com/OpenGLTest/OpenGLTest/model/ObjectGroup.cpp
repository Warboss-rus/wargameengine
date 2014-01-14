#include "ObjectGroup.h"
#include "GameModel.h"

void CObjectGroup::Move(float x, float y, float z)
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->Move(x, y, z);
	}
}

void CObjectGroup::SetCoords(float x, float y, float z)
{
	x -= GetX();
	y -= GetY();
	z -= GetZ();
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->Move(x, y, z);
	}
}

void CObjectGroup::SetCoords(float3 pos)
{
	SetCoords(pos.x, pos.y, pos.z);
}

void CObjectGroup::Rotate(float rotation)
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->Rotate(rotation);
	}
}

float CObjectGroup::GetX() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetX();
	}
	return 0.0;
}

float CObjectGroup::GetY() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetY();
	}
	return 0.0;
}

float CObjectGroup::GetZ() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetZ();
	}
	return 0.0;
}

float CObjectGroup::GetRotation() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetRotation();
	}
	return 0.0;
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

void CObjectGroup::AddChildren(std::shared_ptr<IObject> object)
{
	if(object)
	{
		m_children.push_back(object);
	}
}

void CObjectGroup::RemoveChildren(std::shared_ptr<IObject> object)
{
	if(object == m_children[m_current]) m_current = 0;
	for(unsigned int i =0; i < m_children.size(); ++i)
	{
		if(m_children[i] == object)
		{
			m_children.erase(m_children.begin() + i);
		}
	}
}

bool CObjectGroup::ContainsChildren(std::shared_ptr<IObject> object) const
{
	for(unsigned int i =0; i < m_children.size(); ++i)
	{
		if(m_children[i] == object)
		{
			return true;
		}
	}
	return false;
}

std::shared_ptr<IObject> CObjectGroup::GetChild(size_t index)
{
	return m_children[index];
}

void CObjectGroup::DeleteAll()
{
	CGameModel* model = CGameModel::GetIntanse().lock().get();
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		model->DeleteObjectByPtr(*i);
	}
	m_children.clear();
}

void CObjectGroup::SetCurrent(std::shared_ptr<IObject> object)
{
	for(unsigned int i =0; i < m_children.size(); ++i)
	{
		if(m_children[i] == object)
		{
			m_current = i;
		}
	}
}

void CObjectGroup::SetProperty(std::string const& key, std::string const& value) 
{ 
	for(unsigned int i =0; i < m_children.size(); ++i)
	{
		m_children[i]->SetProperty(key, value);
	}
}
std::string const CObjectGroup::GetProperty(std::string const& key) const 
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetProperty(key);
	}
	return "";
}

void CObjectGroup::SetSelectable(bool selectable)
{
	for(unsigned int i =0; i < m_children.size(); ++i)
	{
		m_children[i]->SetSelectable(selectable);
	}
}

void CObjectGroup::SetMovementLimiter(IMoveLimiter * limiter)
{
	for(unsigned int i =0; i < m_children.size(); ++i)
	{
		m_children[i]->SetMovementLimiter(limiter);
	}
}