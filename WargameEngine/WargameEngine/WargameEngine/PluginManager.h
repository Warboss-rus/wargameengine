#include "Plugin.h"
#include <vector>

namespace wargameEngine
{
class PluginManager
{
public:
	void LoadPlugin(const Path& plugin);
	void LoadFolder(const Path& folder, bool recursive);

private:
	std::vector<Plugin> m_plugins;
};
}