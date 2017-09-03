#include "IPlugin.h"
#include <functional>
#include <memory>
#include <vector>
#include "Typedefs.h"

namespace wargameEngine
{
class PluginManager
{
public:
	using PluginFactory = std::function<std::unique_ptr<IPlugin>(const Path& plugin)>;

	PluginManager(const PluginFactory& factory);

	void LoadPlugin(const Path& plugin);
	void LoadFolder(const Path& folder, bool recursive);

private:
	PluginFactory m_pluginFactory;
	std::vector<std::unique_ptr<IPlugin>> m_plugins;
};
}