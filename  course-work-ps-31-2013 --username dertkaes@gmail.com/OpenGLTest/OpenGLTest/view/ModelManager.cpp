#include "ModelManager.h"
#include "OBJModelFactory.h"
#include "WBMModelFactory.h"
#include "DecalFactory.h"
#include <string>
#include "..\ThreadPool.h"
#include "..\Module.h"

void UseModel(void* data)
{
	sOBJLoader * loader = (sOBJLoader*)data;
	std::swap(*loader->oldmodel, *loader->newModel);
	delete loader;
}

void CModelManager::LoadIfNotExist(std::string const& path)
{
	if(m_models.find(path) == m_models.end())
	{
		unsigned int dotCoord = path.find_last_of('.') + 1;
		std::string extension = path.substr(dotCoord, path.length() - dotCoord);
		if(extension == "obj")
		{
			m_models[path] = new C3DModel();
			sOBJLoader * obj = new sOBJLoader();
			obj->path = sModule::models + path;
			obj->oldmodel = m_models[path];
			CThreadPool::AsyncReadFile(sModule::models + path, LoadObjModel, obj, UseModel);
		}
		if(extension == "wbm")
			m_models[path] = LoadWbmModel(sModule::models + path);
		if(extension == "bmp" || extension == "tga" || extension == "png")
			m_models[path] = LoadDecal(path);
	}
}

void CModelManager::DrawModel(std::string const& path, const std::set<std::string> * hideMeshes)
{
	LoadIfNotExist(path);
	m_models[path]->Draw(hideMeshes);
}

std::shared_ptr<IBounding> CModelManager::GetBoundingBox(std::string const& path)
{
	LoadIfNotExist(path);
	return m_models[path]->GetBounding();
}

CModelManager::~CModelManager()
{
	for(auto i = m_models.begin(); i != m_models.end(); ++i)
	{
		delete i->second;
	}
}