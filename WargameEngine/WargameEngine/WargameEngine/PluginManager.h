#include "Plugin.h"
#include <vector>

class CPluginManager
{
public:
	void LoadPlugin(std::string const& plugin);
	void LoadFolder(std::string const& folder, bool recursive);
private:
	std::vector<CPlugin> m_plugins;
};