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

Bounding LoadBoundingFromFile(const Path& path, float & scale, CVector3f& rotation)
{
	std::ifstream iFile(path);
	Bounding::Compound compound;
	std::string line;
	if (!iFile.good()) return Bounding(compound);
	while (iFile.good())
	{
		iFile >> line;
		if (line == "box")
		{
			CVector3f min, max;
			iFile >> min.x >> min.y >> min.z >> max.x >> max.y >> max.z;
			Bounding::Box box{ min, max };
			compound.items.push_back(Bounding(box));
		}
		if (line == "scale")
		{
			iFile >> scale;
		}
		if (line == "rotationX")
		{
			iFile >> rotation.x;
		}
		if (line == "rotationY")
		{
			iFile >> rotation.y;
		}
		if (line == "rotationZ")
		{
			iFile >> rotation.z;
		}
	}
	iFile.close();
	Bounding bounding = compound.items.size() == 1 ? compound.items[0]: Bounding(compound);
	bounding.scale = scale;
	return bounding;
}

void CModelManager::LoadIfNotExist(const Path& path)
{
	if(m_models.find(path) == m_models.end())
	{
		Path boundingPath = path.substr(0, path.find_last_of('.')) + make_path(L".txt");
		float scale = 1.0;
		CVector3f rotation;
		m_bbManager->AddBounding(path, LoadBoundingFromFile(m_asyncFileProvider->GetModelAbsolutePath(boundingPath), scale, rotation));
		std::shared_ptr<C3DModel> model = std::make_shared<C3DModel>(scale, rotation);
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
			throw std::runtime_error("Cannot load model " + to_string(path) + ". None of installed readers cannot load it");
		}, [=]() {
			 m_models[path]->PreloadTextures(*m_renderer);
		}, [](std::exception const& e) {
			LogWriter::WriteLine(e.what());
		});
	}
}

void CModelManager::DrawModel(const Path& path, IObject* object, bool vertexOnly)
{
	LoadIfNotExist(path);
	std::unique_lock<std::mutex> lk(m_mutex);
	m_models[path]->Draw(*m_renderer, object, vertexOnly, m_gpuSkinning);
}

std::vector<std::string> CModelManager::GetAnimations(const Path& path)
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
