#include "ModelManager.h"
#include "OBJModelFactory.h"
#include <string>
#include <fstream>
#include "../ThreadPool.h"
#include "../Module.h"
#include "../LogWriter.h"
#include "../model/Bounding.h"
#include "../AsyncReadTask.h"
#include "../model/IBoundingBoxManager.h"

CModelManager::CModelManager(IRenderer & renderer, IBoundingBoxManager & bbmanager)
	:m_renderer(&renderer), m_bbManager(&bbmanager)
{
}

std::unique_ptr<IBounding> LoadBoundingFromFile(std::string const& path, double & scale, double * rotation)
{
	std::ifstream iFile(path);
	std::unique_ptr<IBounding> bounding(new CBoundingCompound());
	std::string line;
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
		m_bbManager->AddBoundingBox(path, bounding);
		std::shared_ptr<sOBJLoader> obj = std::make_shared<sOBJLoader>();
		std::shared_ptr<C3DModel> model = std::make_shared<C3DModel>(scale, rotation[0], rotation[1], rotation[2]);
		m_models[path] = model;
		std::function<void(void* data, unsigned int size)> loadingFunc;
		if(extension == "obj")
			loadingFunc = [obj](void* data, unsigned int size) {
				LoadObjModel(data, size, *obj);
			};
		else if(extension == "wbm")
			loadingFunc = [obj](void* data, unsigned int size) {
				LoadWbmModel(data, size, *obj);
			};
		else if (extension == "dae")
			loadingFunc = [obj](void* data, unsigned int size) {
				LoadColladaModel(data, size, *obj);
			};
		else
			LogWriter::WriteLine("Cannot load model " + path + ". Unknown extension " + extension);
		if (loadingFunc)
		{
			std::shared_ptr<AsyncReadTask> readTask = std::make_shared<AsyncReadTask>(sModule::models + path, loadingFunc);
			readTask->AddOnCompleteHandler([=]() {
				model->SetModel(obj->vertices, obj->textureCoords, obj->normals, obj->indexes, obj->materialManager, obj->meshes);
				if (obj->animations.size() > 0) model->SetAnimation(obj->weightsCount, obj->weightsIndexes, obj->weights, obj->joints, obj->animations);
				model->PreloadTextures(*m_renderer);
			});
			readTask->AddOnFailHandler([](std::exception const& e) {
				LogWriter::WriteLine(e.what());
			});
			ThreadPool::AddTask(readTask);
		}
	}
}

void CModelManager::DrawModel(std::string const& path, std::shared_ptr<IObject> object, bool vertexOnly, bool gpuSkinning)
{
	LoadIfNotExist(path);
	m_models[path]->Draw(*m_renderer, object, vertexOnly, gpuSkinning);
}

std::vector<std::string> CModelManager::GetAnimations(std::string const& path)
{
	return m_models[path]->GetAnimations();
}
