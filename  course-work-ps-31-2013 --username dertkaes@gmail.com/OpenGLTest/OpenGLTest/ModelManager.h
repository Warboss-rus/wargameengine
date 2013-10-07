#include "3dModel.h"
#include <map>

class CModelManager
{
public:
	static CModelManager * GetInstance();
	static void FreeInstance();
	void DrawModel(std::string const& path);
	~CModelManager();
protected:
	CModelManager() {}
	CModelManager(CModelManager const& other) {}
private:
	static CModelManager * m_manager;
	std::map<std::string, C3DModel *> m_models;
};