#include "CommandMoveObject.h"
#include "../model/GameModel.h"
#include "../Network.h"
#include "../LogWriter.h"

CCommandMoveObject::CCommandMoveObject(std::shared_ptr<IObject> object, double deltaX, double deltaY) :
	m_deltaX(deltaX), m_deltaY(deltaY), m_pObject(object)
{
}

void CCommandMoveObject::Execute()
{
	m_pObject->Move(m_deltaX, m_deltaY, 0);
}

void CCommandMoveObject::Rollback()
{
	m_pObject->Move(-m_deltaX,-m_deltaY, 0);
}

std::vector<char> CCommandMoveObject::Serialize() const
{
	std::vector<char> result;
	result.resize(21);
	result[0] = 2;//This is a CCommandMoveObject action
	unsigned int address = CNetwork::GetInstance().lock()->GetAddress(m_pObject);
	if (address == 0) CLogWriter::WriteLine("Cannot find this object in the translator");
	memcpy(&result[1], &address, 4);
	memcpy(&result[5], &m_deltaX, 8);
	memcpy(&result[13], &m_deltaY, 8);
	return result;
}
