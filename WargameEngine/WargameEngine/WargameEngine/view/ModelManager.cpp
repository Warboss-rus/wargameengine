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
ModelManager::ModelManager(IRenderer & renderer, model::IBoundingBoxManager & bbmanager, AsyncFileProvider & asyncFileProvider)
	:m_renderer(&renderer), m_bbManager(&bbmanager), m_asyncFileProvider(&asyncFileProvider), m_gpuSkinning(false)
{
}

ModelManager::~ModelManager()
{
}

void ModelManager::LoadIfNotExist(const Path& path)
{
	if (m_models.find(path) == m_models.end())
	{
		std::shared_ptr<C3DModel> model = std::make_shared<C3DModel>(m_bbManager->GetModelScale(path), m_bbManager->GetModelRotation(path));
		m_models[path] = model;
		auto fullPath = m_asyncFileProvider->GetModelAbsolutePath(path);
		m_asyncFileProvider->GetModelAsync(path, [=](void* data, size_t size) {
			unsigned char* charData = reinterpret_cast<unsigned char*>(data);
			for (auto& loader : m_modelReaders)
			{
				if (loader->ModelIsSupported(charData, size, fullPath))
				{
					auto mdl = loader->LoadModel(charData, size, *model, fullPath);
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

void ModelManager::DrawModel(const Path& path, model::IObject* object, bool vertexOnly)
{
	LoadIfNotExist(path);
	std::unique_lock<std::mutex> lk(m_mutex);
	m_models[path]->Draw(*m_renderer, object, vertexOnly, m_gpuSkinning);
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