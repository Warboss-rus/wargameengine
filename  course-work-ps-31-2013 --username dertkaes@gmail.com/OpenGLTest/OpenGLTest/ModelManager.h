#include "3dModel.h"
#include <map>

class CModelManager
{
public:
	C3DModel * GetModel(std::string const& path);
	~CModelManager();
private:
	std::map<std::string, C3DModel *> m_models;
};