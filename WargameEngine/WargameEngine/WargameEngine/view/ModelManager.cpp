#include "ModelManager.h"
#include "OBJModelFactory.h"
#include <string>
#include "../ThreadPool.h"
#include "../Module.h"

void UseModel(void* data)
{
	sOBJLoader * loader = (sOBJLoader*)data;
	loader->model->SetModel(loader->vertices, loader->textureCoords, loader->normals, loader->indexes, loader->materialManager, loader->meshes);
	loader->model->Preload();
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
		std::shared_ptr<IBounding> bounding = LoadBoundingFromFile(sModule::models + boundingPath, scale);
		sOBJLoader * obj = new sOBJLoader();
		obj->model = new C3DModel(bounding, scale);
		m_models[path] = std::shared_ptr<C3DModel>(obj->model);
		if(extension == "obj")
			ThreadPool::AsyncReadFile(sModule::models + path, LoadObjModel, obj, UseModel);
		if(extension == "wbm")
			ThreadPool::AsyncReadFile(sModule::models + path, LoadWbmModel, obj, UseModel);
		if (extension == "dae")
			ThreadPool::AsyncReadFile(sModule::models + path, LoadColladaModel, obj, UseModel);
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