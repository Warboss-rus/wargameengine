#include "CommandChangeProperty.h"
#include "../model/GameModel.h"

CCommandChangeProperty::CCommandChangeProperty(std::shared_ptr<IObject> object, std::string const& key, std::string const& value) :m_key(key), m_newValue(value), m_pObject(object),
	m_oldValue(m_pObject->GetProperty(key))
{

}

void CCommandChangeProperty::Execute()
{
	m_pObject->SetProperty(m_key, m_newValue);
}

void CCommandChangeProperty::Rollback()
{
	m_pObject->SetProperty(m_key, m_oldValue);
}

std::vector<char> CCommandChangeProperty::Serialize() const
{
	std::vector<char> result;
	result.resize(m_key.size() + m_newValue.size() + m_oldValue.size() + 17);
	result[0] = 4;//This is a CCommandChangeProperty action
	void* address = m_pObject.get();
	memcpy(&result[1], &address, 4);
	unsigned int size = m_key.size();
	memcpy(&result[5], &size, 4);
	memcpy(&result[9], m_key.c_str(), size);
	unsigned int begin = size + 9;
	size = m_newValue.size();
	memcpy(&result[begin], &size, 4);
	memcpy(&result[begin + 4], m_newValue.c_str(), size);
	begin += size;
	size = m_oldValue.size();
	memcpy(&result[begin], &size, 4);
	memcpy(&result[begin + 4], m_oldValue.c_str(), size);
	return result;
}