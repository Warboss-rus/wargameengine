#include "ModelManager.h"
#include "OBJModelFactory.h"
#include <string>

void CModelManager::DrawModel(std::string const& path, std::set<std::string> const& hideMeshes)
{
	if(m_models.find(path) == m_models.end())
	{
		m_models[path] = CObjModelCreator::Create(path);
	}
	m_models[path]->Draw(hideMeshes);
}

std::shared_ptr<IBounding> CModelManager::GetBoundingBox(std::string const& path)
{
	if(m_models.find(path) == m_models.end())
	{
		m_models[path] = CObjModelCreator::Create(path);
	}
	return m_models[path]->GetBounding();
}

CModelManager::~CModelManager()
{
	for(auto i = m_models.begin(); i != m_models.end(); ++i)
	{
		delete i->second;
	}
}