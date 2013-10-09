#include "ModelManager.h"
#include "OBJModelFactory.h"
#include <string>

void CModelManager::DrawModel(std::string const& path)
{
	if(m_models.find(path) == m_models.end())
	{
		CObjModelCreator factory;
		m_models[path] = factory.Create(path);
	}
	m_models[path]->Draw();
}

CModelManager::~CModelManager()
{
	for(auto i = m_models.begin(); i != m_models.end(); ++i)
	{
		delete i->second;
	}
}