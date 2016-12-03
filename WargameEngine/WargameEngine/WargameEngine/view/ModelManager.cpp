#include "ModelManager.h"
#include <string>
#include <fstream>
#include "3dModel.h"
#include "IModelReader.h"
#include "../LogWriter.h"
#include "../model/IBoundingBoxManager.h"
#include "../AsyncFileProvider.h"
#include "../Utils.h"

CModelManager::CModelManager(IRenderer & renderer, IBoundingBoxManager & bbmanager, CAsyncFileProvider & asyncFileProvider)
	:m_renderer(&renderer), m_bbManager(&bbmanager), m_asyncFileProvider(&asyncFileProvider), m_gpuSkinning(false)
{
}

CModelManager::~CModelManager()
{
}

sBounding LoadBoundingFromFile(std::wstring const& path, float & scale, double * rotation)
{
	std::ifstream iFile;
	OpenFile(iFile, path);
	sBounding::sCompound compound;
	std::string line;
	if (!iFile.good()) return sBounding(compound);
	while (iFile.good())
	{
		iFile >> line;
		if (line == "box")
		{
			CVector3f min, max;
			iFile >> min.x >> min.y >> min.z >> max.x >> max.y >> max.z;
			sBounding::sBox box{ min, max };
			compound.items.push_back(sBounding(box));
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
	sBounding bounding = compound.items.size() == 1 ? compound.items[0]: sBounding(compound);
	bounding.scale = scale;
	return bounding;
}

void CModelManager::LoadIfNotExist(std::wstring const& path)
{
	if(m_models.find(path) == m_models.end())
	{
		std::wstring boundingPath = path.substr(0, path.find_last_of('.')) + L".txt";
		float scale = 1.0;
		double rotation[3] = { 0.0, 0.0, 0.0 };
		m_bbManager->AddBounding(path, LoadBoundingFromFile(m_asyncFileProvider->GetModelAbsolutePath(boundingPath), scale, rotation));
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
					return;
				}
			}
			throw std::runtime_error("Cannot load model " + WStringToUtf8(path) + ". None of installed readers cannot load it");
		}, [=]() {
			 m_models[path]->PreloadTextures(*m_renderer);
		}, [](std::exception const& e) {
			LogWriter::WriteLine(e.what());
		});
	}
}

void CModelManager::DrawModel(std::wstring const& path, IObject* object, bool vertexOnly)
{
	LoadIfNotExist(path);
	std::unique_lock<std::mutex> lk(m_mutex);
	m_models[path]->Draw(*m_renderer, object, vertexOnly, m_gpuSkinning);
}

std::vector<std::string> CModelManager::GetAnimations(std::wstring const& path)
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

void CModelManager::Reset()
{
	m_models.clear();
}
