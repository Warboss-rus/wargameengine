#include <map>
#include <set>
#include <memory>
#include <vector>
#include <mutex>

class IObject;
class IRenderer;
class IBoundingBoxManager;
class CAsyncFileProvider;
class IModelReader;
class C3DModel;
class IShaderManager;

class CModelManager
{
public:
	CModelManager(IRenderer & renderer, IBoundingBoxManager & bbmanager, CAsyncFileProvider & asyncFileProvider);
	~CModelManager();
	void DrawModel(std::string const& path, std::shared_ptr<IObject> object, bool vertexOnly = false, IShaderManager * shaderManager = nullptr);
	void LoadIfNotExist(std::string const& path);
	std::vector<std::string> GetAnimations(std::string const& path);
	void EnableGPUSkinning(bool enable);
	void RegisterModelReader(std::unique_ptr<IModelReader> && reader);
	void Reset(IBoundingBoxManager & bbmanager);
private:
	std::map<std::string, std::shared_ptr<C3DModel>> m_models;
	std::vector<std::unique_ptr<IModelReader>> m_modelReaders;
	IRenderer * m_renderer;
	IBoundingBoxManager * m_bbManager;
	CAsyncFileProvider * m_asyncFileProvider;
	std::mutex m_mutex;
	bool m_gpuSkinning;
};