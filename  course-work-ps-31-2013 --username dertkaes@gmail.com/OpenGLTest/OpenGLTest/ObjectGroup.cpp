#include "ObjectGroup.h"
#include "model\GameModel.h"

void CObjectGroup::Move(double x, double y, double z)
{
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
	if(m_children.begin() != m_children.end())
	{
		return m_children.begin()->get()->GetX();
	}
	return 0.0;
}

double CObjectGroup::GetY() const
{
	if(m_children.begin() != m_children.end())
	{
		return m_children.begin()->get()->GetY();
	}
	return 0.0;
}

double CObjectGroup::GetZ() const
{
	if(m_children.begin() != m_children.end())
	{
		return m_children.begin()->get()->GetZ();
	}
	return 0.0;
}

double CObjectGroup::GetRotation() const
{
	if(m_children.begin() != m_children.end())
	{
		return m_children.begin()->get()->GetRotation();
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