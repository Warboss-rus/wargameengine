#include <map>
#include <set>
#include <memory>
#include <vector>
#include <mutex>
#include "../Typedefs.h"

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

class ModelManager
{
public:
	ModelManager(IRenderer & renderer, model::IBoundingBoxManager & bbmanager, AsyncFileProvider & asyncFileProvider);
	~ModelManager();
	void DrawModel(const Path& path, model::IObject* object, bool vertexOnly = false);
	void LoadIfNotExist(const Path& path);
	std::vector<std::string> GetAnimations(const Path& path);
	void EnableGPUSkinning(bool enable);
	void RegisterModelReader(std::unique_ptr<IModelReader> && reader);
	void Reset();
private:
	std::map<Path, std::shared_ptr<C3DModel>> m_models;
	std::vector<std::unique_ptr<IModelReader>> m_modelReaders;
	IRenderer * m_renderer;
	model::IBoundingBoxManager * m_bbManager;
	AsyncFileProvider * m_asyncFileProvider;
	std::mutex m_mutex;
	bool m_gpuSkinning;
};
}
}