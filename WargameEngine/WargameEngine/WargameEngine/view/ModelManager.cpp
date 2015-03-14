#include "ModelManager.h"
#include "OBJModelFactory.h"
#include <string>
#include <fstream>
#include "../ThreadPool.h"
#include "../Module.h"
#include "../model/Object.h"
#include "../LogWriter.h"
#include "../model/Bounding.h"
#include "../model/GameModel.h"

void UseModel(void* data)
{
	sOBJLoader * loader = (sOBJLoader*)data;
	loader->model->SetModel(loader->vertices, loader->textureCoords, loader->normals, loader->indexes, loader->materialManager, loader->meshes);
	if(loader->animations.size() > 0) loader->model->SetAnimation(loader->weightsCount, loader->weightsIndexes, loader->weights, loader->joints, loader->animations);
	loader->model->PreloadTextures();
	delete loader;
}

std::unique_ptr<IBounding> LoadBoundingFromFile(std::string const& path, double & scale, double * rotation)
{
	std::ifstream iFile(path);
	std::unique_ptr<IBounding> bounding(new CBoundingCompound());
	std::string line;
	unsigned int count = 0;
	if (!iFile.good()) return NULL;
	while (iFile.good())
	{
		iFile >> line;
		if (line == "box")
		{
			double min[3], max[3];
			iFile >> min[0] >> min[1] >> min[2] >> max[0] >> max[1] >> max[2];
			CBoundingCompound * compound = (CBoundingCompound *)bounding.get();
			compound->AddChild(std::unique_ptr<IBounding>(new CBoundingBox(min, max)));
		}
		if (line == "scale")
		{
			iFile >> scale;
		}
		if (line == "rotationX")
		{
			iFile >> rotation[0];
		}
		if (line == "rotationY")
		{
			iFile >> rotation[1];
		}
		if (line == "rotationZ")
		{
			iFile >> rotation[2];
		}
	}
	iFile.close();
	CBoundingCompound * compound = (CBoundingCompound *)bounding.get();
	compound->SetScale(scale);
	return bounding;
}

void CModelManager::LoadIfNotExist(std::string const& path)
{
	if(m_models.find(path) == m_models.end())
	{
		unsigned int dotCoord = path.find_last_of('.') + 1;
		std::string extension = path.substr(dotCoord, path.length() - dotCoord);
		std::string boundingPath = path.substr(0, path.find_last_of('.')) + ".txt";
		double scale = 1.0;
		double rotation[3] = { 0.0, 0.0, 0.0 };
		std::shared_ptr<IBounding> bounding = LoadBoundingFromFile(sModule::models + boundingPath, scale, rotation);
		CGameModel::GetInstance().lock()->AddBoundingBox(path, bounding);
		sOBJLoader * obj = new sOBJLoader();
		obj->model = new C3DModel(scale, rotation[0], rotation[1], rotation[2]);
		m_models[path] = std::shared_ptr<C3DModel>(obj->model);
		if(extension == "obj")
			ThreadPool::AsyncReadFile(sModule::models + path, LoadObjModel, obj, UseModel);
		else if(extension == "wbm")
			ThreadPool::AsyncReadFile(sModule::models + path, LoadWbmModel, obj, UseModel);
		else if (extension == "dae")
			ThreadPool::AsyncReadFile(sModule::models + path, LoadColladaModel, obj, UseModel);
		else
			LogWriter::WriteLine("Cannot load model " + path + ". Unknown extension " + extension);
	}
}

void CModelManager::DrawModel(std::string const& path, std::shared_ptr<IObject> object, bool vertexOnly, bool gpuSkinning)
{
	LoadIfNotExist(path);
	m_models[path]->Draw(object, vertexOnly, gpuSkinning);
}

std::vector<std::string> CModelManager::GetAnimations(std::string const& path)
{
	return m_models[path]->GetAnimations();
}