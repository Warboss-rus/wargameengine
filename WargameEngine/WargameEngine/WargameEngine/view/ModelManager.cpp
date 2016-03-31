#include "ModelManager.h"
#include <string>
#include <fstream>
#include "3dModel.h"
#include "IModelReader.h"
#include "../LogWriter.h"
#include "../model/Bounding.h"
#include "../model/IBoundingBoxManager.h"
#include "../AsyncFileProvider.h"

CModelManager::CModelManager(IRenderer & renderer, IBoundingBoxManager & bbmanager, CAsyncFileProvider & asyncFileProvider)
	:m_renderer(&renderer), m_bbManager(&bbmanager), m_asyncFileProvider(&asyncFileProvider), m_gpuSkinning(false)
{
}

CModelManager::~CModelManager()
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
		std::string boundingPath = path.substr(0, path.find_last_of('.')) + ".txt";
		double scale = 1.0;
		double rotation[3] = { 0.0, 0.0, 0.0 };
		std::shared_ptr<IBounding> bounding = LoadBoundingFromFile(m_asyncFileProvider->GetModelAbsolutePath(boundingPath), scale, rotation);
		m_bbManager->AddBoundingBox(path, bounding);
		std::shared_ptr<C3DModel> model = std::make_shared<C3DModel>(scale, rotation[0], rotation[1], rotation[2]);
		m_models[path] = model;
		auto fullPath = m_asyncFileProvider->GetModelAbsolutePath(path);		
		m_asyncFileProvider->GetModelAsync(path, [=](void* data, size_t size) {
			unsigned char* charData = reinterpret_cast<unsigned char*>(data);
			for (auto& loader : m_modelReaders)
			{
				if (loader->ModelIsSupported(charData, size, fullPath))
				{
					auto mdl = loader->LoadModel(charData, size, *model, fullPath);;
					std::unique_lock<std::mutex> lk(m_mutex);
					m_models[path] = std::move(mdl);
				}
			}
			throw std::runtime_error("Cannot load model " + path + ". None of installed readers cannot load it");
		}, [=]() {
			 m_models[path]->PreloadTextures(*m_renderer);
		}, [](std::exception const& e) {
			LogWriter::WriteLine(e.what());
		});
	}
}

void CModelManager::DrawModel(std::string const& path, std::shared_ptr<IObject> object, bool vertexOnly, IShaderManager * shaderManager)
{
	LoadIfNotExist(path);
	std::unique_lock<std::mutex> lk(m_mutex);
	m_models[path]->Draw(*m_renderer, object, vertexOnly, m_gpuSkinning, shaderManager);
}

std::vector<std::string> CModelManager::GetAnimations(std::string const& path)
{
	return m_models[path]->GetAnimations();
}

void CModelManager::EnableGPUSkinning(bool enable)
{
	m_gpuSkinning = enable;
}

void CModelManager::RegisterModelReader(std::unique_ptr<IModelReader> && reader)
{
	m_modelReaders.push_back(std::move(reader));
}

void CModelManager::Reset(IBoundingBoxManager & bbmanager)
{
	m_models.clear();
	m_bbManager = &bbmanager;
}
