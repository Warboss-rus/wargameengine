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
	loader->model->SetModel(loader->vertices, loader->textureCoords, loader->normals, loader->indexes, loader->materialManager, loader->meshes);
	delete loader;
}

void CModelManager::LoadIfNotExist(std::string const& path)
{
	if(m_models.find(path) == m_models.end())
	{
		unsigned int dotCoord = path.find_last_of('.') + 1;
		std::string extension = path.substr(dotCoord, path.length() - dotCoord);
		std::string boundingPath = path.substr(0, path.find_last_of('.')) + ".txt";
		double scale = 1.0;
		std::shared_ptr<IBounding> bounding = LoadBoundingFromFile(boundingPath, scale);
		m_models[path] = std::shared_ptr<C3DModel>(new C3DModel(bounding, scale));
		if(extension == "obj")
		{
			sOBJLoader * obj = new sOBJLoader();
			obj->model = m_models[path].get();
			ThreadPool::AsyncReadFile(sModule::models + path, LoadObjModel, obj, UseModel);
		}
		if(extension == "wbm")
			m_models[path] = std::shared_ptr<C3DModel>(LoadWbmModel(sModule::models + path));
		if(extension == "bmp" || extension == "tga" || extension == "png")
			m_models[path] = std::shared_ptr<C3DModel>(LoadDecal(path));
	}
}

void CModelManager::DrawModel(std::string const& path, const std::set<std::string> * hideMeshes, bool vertexOnly)
{
	LoadIfNotExist(path);
	m_models[path]->Draw(hideMeshes, vertexOnly);
}

std::shared_ptr<IBounding> CModelManager::GetBoundingBox(std::string const& path)
{
	LoadIfNotExist(path);
	return m_models[path]->GetBounding();
}