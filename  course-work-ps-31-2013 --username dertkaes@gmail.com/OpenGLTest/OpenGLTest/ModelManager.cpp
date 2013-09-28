#include "ModelManager.h"
#include <string>

C3DModel * CModelManager::GetModel(std::string const& path)
{
	if(m_models.find(path) == m_models.end())
	{
		m_models[path] = new C3DModel(path);
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