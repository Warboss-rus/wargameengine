#include "ModelManager.h"
#include <string>
#include <fstream>
#include "3dModel.h"
#include "IModelReader.h"
#include "../LogWriter.h"
#include "../model/IBoundingBoxManager.h"
#include "../AsyncFileProvider.h"
#include "../Utils.h"

namespace wargameEngine
{
namespace view
{
ModelManager::ModelManager(model::IBoundingBoxManager & bbmanager, AsyncFileProvider & asyncFileProvider)
	: m_bbManager(&bbmanager), m_asyncFileProvider(&asyncFileProvider), m_gpuSkinning(false)
{
}

ModelManager::~ModelManager()
{
}

void ModelManager::LoadIfNotExist(const Path& path, TextureManager& textureManager)
{
	if (m_models.find(path) == m_models.end())
	{
		auto& model = *m_models.emplace(std::make_pair(path, std::make_unique<C3DModel>(m_bbManager->GetModelScale(path), m_bbManager->GetModelRotation(path)))).first->second;
		auto fullPath = m_asyncFileProvider->GetModelAbsolutePath(path);
		m_asyncFileProvider->GetModelAsync(path, [=, &model](void* data, size_t size) {
			unsigned char* charData = reinterpret_cast<unsigned char*>(data);
			for (auto& loader : m_modelReaders)
			{
				if (loader->ModelIsSupported(charData, size, fullPath))
				{
					auto mdl = loader->LoadModel(charData, size, model, fullPath);
					std::unique_lock<std::mutex> lk(m_mutex);
					m_models[path] = std::move(mdl);
					return;
				}
			}
			throw std::runtime_error("Cannot load model " + path.string() + ". None of installed readers cannot load it");
		}, [=, &textureManager]() {
			m_models[path]->PreloadTextures(textureManager);
		}, [](std::exception const& e) {
			LogWriter::WriteLine(e.what());
		});
	}
}

void ModelManager::GetModelMeshes(const Path& path, IRenderer & renderer, TextureManager& textureManager, model::IObject* object, MeshList& meshesVec)
{
	LoadIfNotExist(path, textureManager);
	std::unique_lock<std::mutex> lk(m_mutex);
	m_models[path]->GetMeshes(renderer, textureManager, object, m_gpuSkinning, meshesVec);
}

std::vector<std::string> ModelManager::GetAnimations(const Path& path)
{
	return m_models[path]->GetAnimations();
}

void ModelManager::EnableGPUSkinning(bool enable)
{
	m_gpuSkinning = enable;
}

void ModelManager::RegisterModelReader(std::unique_ptr<IModelReader> && reader)
{
	m_modelReaders.push_back(std::move(reader));
}

void ModelManager::Reset()
{
	m_models.clear();
}
}
}