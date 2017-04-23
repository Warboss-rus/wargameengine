#include <map>
#include <set>
#include <memory>
#include <vector>
#include <mutex>
#include "../Typedefs.h"

class IObject;
class IRenderer;
class IBoundingBoxManager;
class CAsyncFileProvider;
class IModelReader;
class C3DModel;

class CModelManager
{
public:
	CModelManager(IRenderer & renderer, IBoundingBoxManager & bbmanager, CAsyncFileProvider & asyncFileProvider);
	~CModelManager();
	void DrawModel(const Path& path, IObject* object, bool vertexOnly = false);
	void LoadIfNotExist(const Path& path);
	std::vector<std::string> GetAnimations(const Path& path);
	void EnableGPUSkinning(bool enable);
	void RegisterModelReader(std::unique_ptr<IModelReader> && reader);
	void Reset();
private:
	std::map<Path, std::shared_ptr<C3DModel>> m_models;
	std::vector<std::unique_ptr<IModelReader>> m_modelReaders;
	IRenderer * m_renderer;
	IBoundingBoxManager * m_bbManager;
	CAsyncFileProvider * m_asyncFileProvider;
	std::mutex m_mutex;
	bool m_gpuSkinning;
};