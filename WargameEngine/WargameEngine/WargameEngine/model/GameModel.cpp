#include "GameModel.h"
#include "../view/ModelManager.h"
#include "../model/ObjectGroup.h"
#include "3dObject.h"
#include <cstring>

std::shared_ptr<CGameModel> CGameModel::m_instanse = NULL;

CGameModel::~CGameModel(void)
{
}

std::weak_ptr<CGameModel> CGameModel::GetInstance()
{
	if (!m_instanse)
	{
		m_instanse.reset(new CGameModel());
	}
	return std::weak_ptr<CGameModel>(m_instanse);
}

void CGameModel::FreeInstance() 
{
	m_instanse.reset();
}

unsigned long CGameModel::GetObjectCount() const
{
	return m_objects.size();
}

std::shared_ptr<const IObject> CGameModel::Get3DObject(unsigned long number) const
{
	return m_objects[number];
}

std::shared_ptr<IObject> CGameModel::Get3DObject(unsigned long number)
{
	return m_objects[number];
}

std::shared_ptr<IObject> CGameModel::Get3DObject(IObject * object)
{
	for (auto i = m_objects.begin(); i != m_objects.end(); ++i)
	{
		if (i->get() == object)
		{
			return *i;
		}
	}
	return std::shared_ptr<IObject>();
}

void CGameModel::AddObject(std::shared_ptr<IObject> pObject)
{
	m_objects.push_back(pObject);
}

void CGameModel::SelectObject(std::shared_ptr<IObject> pObject)
{
	m_selectedObject = pObject;
}

std::shared_ptr<const IObject> CGameModel::GetSelectedObject() const
{
	return m_selectedObject;
}

std::shared_ptr<IObject> CGameModel::GetSelectedObject()
{
	return m_selectedObject;
}

void CGameModel::DeleteObjectByPtr(std::shared_ptr<IObject> pObject)
{
	if(IsGroup(pObject.get()))
	{
		CObjectGroup* group = (CObjectGroup*)pObject.get();
		group->DeleteAll();
	}
	if(pObject == m_selectedObject) m_selectedObject = NULL;
	for(unsigned int i = 0; i < m_objects.size(); ++i)
	{
		if(m_objects[i].get() == pObject.get())
		{
			
			m_objects.erase(m_objects.begin() + i);
		}
	}
}

bool CGameModel::IsGroup(IObject* object)
{
	return dynamic_cast<CObjectGroup *>(object) != NULL;
}

void CGameModel::Clear()
{
	m_objects.clear();
	m_properties.clear();
}

std::string const CGameModel::GetProperty(std::string const& key) 
{
	if (m_properties.find(key) != m_properties.end())
		return m_properties.find(key)->second;
	return "";
}

void CGameModel::SetProperty(std::string const& key, std::string const& value)
{ 
	m_properties[key] = value; 
}

std::map<std::string, std::string> const&  CGameModel::GetAllProperties() const
{ 
	return m_properties; 
}

std::vector<char> PackProperties(std::map<std::string, std::string> const&properties)
{
	std::vector<char> result;
	result.resize(4);
	*((unsigned int*)&result[0]) = properties.size();
	for (auto i = properties.begin(); i != properties.end(); ++i)
	{
		unsigned int begin = result.size();
		result.resize(begin + 10 + i->first.size() + i->second.size());
		*((unsigned int*)&result[begin]) = i->first.size() + 1;
		memcpy(&result[begin + 4], i->first.c_str(), i->first.size() + 1);
		begin += i->first.size() + 5;
		*((unsigned int*)&result[begin]) = i->second.size() + 1;
		memcpy(&result[begin + 4], i->second.c_str(), i->second.size() + 1);
	}
	return result;
}

std::vector<char> CGameModel::GetState() const
{
	std::vector<char> result;
	result.resize(9);
	result[0] = 1;
	unsigned int count = GetObjectCount();
	*((unsigned int*)&result[5]) = count;
	for (unsigned int i = 0; i < count; ++i)
	{
		const IObject * object = Get3DObject(i).get();
		std::vector<char> current;
		std::string path = object->GetPathToModel();
		current.resize(36 + path.size() + 1, 0);
		*((double*)&current[0]) = object->GetX();
		*((double*)&current[8]) = object->GetY();
		*((double*)&current[16]) = object->GetZ();
		*((double*)&current[24]) = object->GetRotation();
		*((unsigned int*)&current[32]) = path.size() + 1;
		memcpy(&current[36], path.c_str(), path.size() + 1);
		std::vector<char> properties = PackProperties(object->GetAllProperties());
		current.insert(current.end(), properties.begin(), properties.end());
		result.insert(result.end(), current.begin(), current.end());
	}
	std::vector<char> globalProperties = PackProperties(GetAllProperties());
	result.insert(result.end(), globalProperties.begin(), globalProperties.end());
	*((unsigned int*)&result[1]) = result.size();
	return result;
}

void CGameModel::SetState(char* data)
{
	unsigned int count = *(unsigned int*)&data[0];
	unsigned int current = 4;
	CGameModel * model = CGameModel::GetInstance().lock().get();
	model->Clear();
	for (unsigned int i = 0; i < count; ++i)
	{
		double x = *((double*)&data[current]);
		double y = *((double*)&data[current + 8]);
		double z = *((double*)&data[current + 16]);
		double rotation = *((double*)&data[current + 24]);
		unsigned int pathSize = *((unsigned int*)&data[current + 32]);
		char * path = new char[pathSize];
		memcpy(path, &data[current + 36], pathSize);
		IObject* object = new C3DObject(path, x, y, rotation);
		model->AddObject(std::shared_ptr<IObject>(object));
		delete[] path;
		current += 36 + pathSize;
		unsigned int propertiesCount = *((unsigned int*)&data[current]);
		current += 4;
		for (unsigned int i = 0; i < propertiesCount; ++i)
		{
			unsigned int firstSize = *((unsigned int*)&data[current]);
			char * first = new char[firstSize];
			memcpy(first, &data[current + 4], firstSize);
			current += firstSize + 4;
			unsigned int secondSize = *((unsigned int*)&data[current]);
			char * second = new char[secondSize];
			memcpy(second, &data[current + 4], secondSize);
			current += secondSize + 4;
			object->SetProperty(first, second);
			delete[] first;
			delete[] second;
		}
	}
	unsigned int globalPropertiesCount = *((unsigned int*)&data[current]);
	current += 4;
	for (unsigned int i = 0; i < globalPropertiesCount; ++i)
	{
		unsigned int firstSize = *((unsigned int*)&data[current]);
		char * first = new char[firstSize];
		memcpy(first, &data[current + 4], firstSize);
		current += firstSize + 4;
		unsigned int secondSize = *((unsigned int*)&data[current]);
		char * second = new char[secondSize];
		memcpy(second, &data[current + 4], secondSize);
		current += secondSize + 4;
		model->SetProperty(first, second);
		delete[] first;
		delete[] second;
	}
}