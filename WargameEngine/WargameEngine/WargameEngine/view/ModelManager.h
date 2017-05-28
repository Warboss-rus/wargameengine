#include <unordered_map>
#include <set>
#include <memory>
#include <vector>
#include <mutex>
#include "../Typedefs.h"
#include "DrawableMesh.h"

namespace wargameEngine
{
class AsyncFileProvider;

namespace model
{
class IObject;
class IBoundingBoxManager;
}

namespace view
{
class IRenderer;
class IModelReader;
class C3DModel;
class TextureManager;

class ModelManager
{
public:
	ModelManager(model::IBoundingBoxManager & bbmanager, AsyncFileProvider & asyncFileProvider);
	~ModelManager();
	void GetModelMeshes(const Path& path, IRenderer & renderer, TextureManager& textureManager, model::IObject* object, std::vector<DrawableMesh>& meshesVec);
	void LoadIfNotExist(const Path& path, TextureManager& textureManager);
	std::vector<std::string> GetAnimations(const Path& path);
	void EnableGPUSkinning(bool enable);
	void RegisterModelReader(std::unique_ptr<IModelReader> && reader);
	void Reset();
private:
	std::unordered_map<Path, std::unique_ptr<C3DModel>> m_models;
	std::vector<std::unique_ptr<IModelReader>> m_modelReaders;
	model::IBoundingBoxManager * m_bbManager;
	AsyncFileProvider * m_asyncFileProvider;
	std::mutex m_mutex;
	bool m_gpuSkinning;
};
}
}