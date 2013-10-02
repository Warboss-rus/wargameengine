#include "ModelManager.h"
#include "OBJModelFactory.h"
#include <string>

C3DModel * CModelManager::GetModel(std::string const& path)
{
	if(m_models.find(path) == m_models.end())
	{
		CObjModelCreator factory;
		m_models[path] = factory.Create(path);
	}
	return m_models[path];
}

CModelManager::~CModelManager()
{
	for(auto i = m_models.begin(); i != m_models.end(); ++i)
	{
		delete i->second;
	}
}