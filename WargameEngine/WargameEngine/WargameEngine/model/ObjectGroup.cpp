#include "ObjectGroup.h"
#include "GameModel.h"

CObjectGroup::CObjectGroup() :m_current(0)
{

}

std::string CObjectGroup::GetPathToModel() const
{
	if(m_children.empty()) return "";
	return m_children[m_current]->GetPathToModel();
}

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

void CObjectGroup::SetCoords(CVector3d const& coords)
{
	double x = coords.x - GetX();
	double y = coords.y - GetY();
	double z = coords.z - GetZ();
	for (auto i = m_children.begin(); i != m_children.end(); ++i)
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

CVector3d CObjectGroup::GetCoords() const
{
	return CVector3d(GetX(), GetY(), GetZ());
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
	CGameModel* model = CGameModel::GetInstance().lock().get();
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

std::shared_ptr<IObject> CObjectGroup::GetCurrent() const
{
	return m_children[m_current];
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

bool CObjectGroup::IsSelectable() const
{
	return true;
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

std::map<std::string, std::string> const& CObjectGroup::GetAllProperties() const
{
	return m_children[0]->GetAllProperties();
}

bool CObjectGroup::CastsShadow() const
{
	if (m_children.empty()) return true;
	return m_children[m_current]->CastsShadow();
}

void CObjectGroup::PlayAnimation(std::string const& animation, sAnimation::eLoopMode loop, float speed)
{
	for (unsigned int i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->PlayAnimation(animation, loop, speed);
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

void CObjectGroup::AddSecondaryModel(std::string const& model)
{
	for (unsigned int i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->AddSecondaryModel(model);
	}
}

void CObjectGroup::RemoveSecondaryModel(std::string const& model)
{
	for (unsigned int i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->RemoveSecondaryModel(model);
	}
}

unsigned int CObjectGroup::GetSecondaryModelsCount() const
{
	if (m_children.empty()) return 0;
	return m_children[m_current]->GetSecondaryModelsCount();
}

std::string CObjectGroup::GetSecondaryModel(unsigned int index) const
{
	if (m_children.empty()) return "";
	return m_children[m_current]->GetSecondaryModel(index);
}

sAnimation::eLoopMode CObjectGroup::GetAnimationLoop() const
{
	if (m_children.empty()) return sAnimation::eLoopMode::NONLOOPING;
	return m_children[m_current]->GetAnimationLoop();
}

float CObjectGroup::GetAnimationSpeed() const
{
	if (m_children.empty()) return 1.0f;
	return m_children[m_current]->GetAnimationSpeed();
}

void CObjectGroup::GoTo(CVector3d const& coords, double speed, std::string const& animation, float animationSpeed)//needs to be reworked
{
	for (unsigned int i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->GoTo(coords, speed, animation, animationSpeed);
	}
}

void CObjectGroup::Update()
{
	for (unsigned int i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->Update();
	}
}

void CObjectGroup::ApplyTeamColor(std::string const& suffix, unsigned char r, unsigned char g, unsigned char b)
{
	for (unsigned int i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->ApplyTeamColor(suffix, r, g, b);
	}
}

void CObjectGroup::ReplaceTexture(std::string const& oldTexture, std::string const& newTexture)
{
	for (unsigned int i = 0; i < m_children.size(); ++i)
	{
		m_children[i]->ReplaceTexture(oldTexture, newTexture);
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

std::map<std::string, std::string> const& CObjectGroup::GetReplaceTextures() const
{ 
	if (m_children.empty())
	{
		static std::map<std::string, std::string> dummy;
		return dummy;
	}
	return m_children[m_current]->GetReplaceTextures();
}