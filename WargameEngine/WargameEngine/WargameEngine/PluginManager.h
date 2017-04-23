#include "Plugin.h"
#include <vector>

class CPluginManager
{
public:
	void LoadPlugin(const Path& plugin);
	void LoadFolder(const Path& folder, bool recursive);
private:
	std::vector<CPlugin> m_plugins;
};