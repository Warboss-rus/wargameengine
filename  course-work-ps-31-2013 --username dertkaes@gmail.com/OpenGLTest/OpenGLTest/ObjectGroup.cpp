#include "ObjectGroup.h"
#include "model\GameModel.h"

void CObjectGroup::Move(double x, double y, double z)
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->Move(x, y, z);
	}
}

void CObjectGroup::SetCoords(double x, double y, double z)
{
	x -= GetX();
	y -= GetY();
	z -= GetZ();
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->Move(x, y, z);
	}
}

void CObjectGroup::Rotate(double rotation)
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		i->get()->Rotate(rotation);
	}
}

double CObjectGroup::GetX() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetX();
	}
	return 0.0;
}

double CObjectGroup::GetY() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetY();
	}
	return 0.0;
}

double CObjectGroup::GetZ() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetZ();
	}
	return 0.0;
}

double CObjectGroup::GetRotation() const
{
	if(m_current < m_children.size())
	{
		return m_children[m_current]->GetRotation();
	}
	return 0.0;
}

std::set<std::string> const& CObjectGroup::GetHiddenMeshes() const
{
	return std::set<std::string>();
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
	m_children.push_back(object);
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

void CObjectGroup::SetRelocatable(bool relocatable)
{
	for(unsigned int i =0; i < m_children.size(); ++i)
	{
		m_children[i]->SetRelocatable(relocatable);
	}
}