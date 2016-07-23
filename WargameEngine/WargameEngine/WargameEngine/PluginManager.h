#include "Plugin.h"
#include <vector>

class CPluginManager
{
public:
	void LoadPlugin(std::wstring const& plugin);
	void LoadFolder(std::wstring const& folder, bool recursive);
private:
	std::vector<CPlugin> m_plugins;
};