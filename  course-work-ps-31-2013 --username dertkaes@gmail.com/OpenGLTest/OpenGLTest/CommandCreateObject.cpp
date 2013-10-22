#include "CommandCreateObject.h"
#include "model\GameModel.h"
#include "3dObject.h"


CCommandCreateObject::CCommandCreateObject(std::string const& model, double x, double y, double rotation):
	m_model(model), m_x(x), m_y(y), m_rotation(rotation) {}

void CCommandCreateObject::Execute()
{
	m_pObject.reset(new C3DObject(m_model, m_x, m_y, m_rotation)); 
	CGameModel::GetIntanse().lock()->AddObject(m_pObject);
}

void CCommandCreateObject::Rollback()
{
	CGameModel::GetIntanse().lock()->DeleteObjectByPtr(m_pObject);
}