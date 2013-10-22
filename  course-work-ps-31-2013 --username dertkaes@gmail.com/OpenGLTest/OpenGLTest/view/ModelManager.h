#include "3dModel.h"
#include <map>

class CModelManager
{
public:
	CModelManager() {}
	void DrawModel(std::string const& path);
	std::shared_ptr<IBounding> GetBoundingBox(std::string const& path);
	~CModelManager();
protected:
	CModelManager(CModelManager const& other) {}
private:
	std::map<std::string, C3DModel *> m_models;
};