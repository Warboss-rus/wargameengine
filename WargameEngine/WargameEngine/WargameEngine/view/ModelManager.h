#include "3dModel.h"
#include <map>
#include <set>
class IObject;

class CModelManager
{
public:
	CModelManager() {}
	void DrawModel(std::string const& path, std::shared_ptr<IObject> object, bool vertexOnly = false, bool gpuSkinning = false);
	void LoadIfNotExist(std::string const& path);
	std::vector<std::string> GetAnimations(std::string const& path);
protected:
	CModelManager(CModelManager const& other) {}
private:
	std::map<std::string, std::shared_ptr<C3DModel>> m_models;
};