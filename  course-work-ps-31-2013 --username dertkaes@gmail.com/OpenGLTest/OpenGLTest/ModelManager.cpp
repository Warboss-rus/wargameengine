#include "ModelManager.h"
#include "OBJModelFactory.h"
#include <string>

CModelManager * CModelManager::m_manager = NULL;

CModelManager * CModelManager::GetInstance()
{
	if(!m_manager)
	{
		m_manager = new CModelManager;
	}
	return m_manager;
}

void CModelManager::FreeInstance()
{
	delete m_manager;
	m_manager = NULL;
}

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