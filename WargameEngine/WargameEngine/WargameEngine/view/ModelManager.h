#include "3dModel.h"
#include <map>
#include <set>

class IObject;
class IRenderer;
class IBoundingBoxManager;

class CModelManager
{
public:
	CModelManager(IRenderer & renderer, IBoundingBoxManager & bbmanager);
	void DrawModel(std::string const& path, std::shared_ptr<IObject> object, bool vertexOnly = false, IShaderManager * shaderManager = nullptr);
	void LoadIfNotExist(std::string const& path);
	std::vector<std::string> GetAnimations(std::string const& path);
	void EnableGPUSkinning(bool enable);
protected:
	CModelManager(CModelManager const& other) = default;
private:
	std::map<std::string, std::shared_ptr<C3DModel>> m_models;
	IRenderer * m_renderer;
	IBoundingBoxManager * m_bbManager;
	bool m_gpuSkinning;
};