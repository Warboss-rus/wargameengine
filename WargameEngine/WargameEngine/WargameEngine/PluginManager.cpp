#include "PluginManager.h"
#include "LogWriter.h"
#include "OSSpecific.h"
#include "Utils.h"

namespace wargameEngine
{
void PluginManager::LoadPlugin(const Path& file)
{
	Plugin plugin(file);
	PluginGetTypeFunction typeFunc = reinterpret_cast<PluginGetTypeFunction>(plugin.GetFunction("GetType"));
	PluginGetClassFunction instanceFunc = reinterpret_cast<PluginGetClassFunction>(plugin.GetFunction("GetClass"));
	if (!typeFunc || !instanceFunc)
	{
		LogWriter::WriteLine("A plugin " + to_string(file) + " don't have a GetType and GetClass functions.");
		return;
	}
	std::string type = typeFunc();
	if (type == "image")
	{
		//Register image type plugin
	}
	else if (type == "sound")
	{
		//Register sound type plugin
	}
	else if (type == "model")
	{
		//Register model type plugin
	}
	else
	{
		LogWriter::WriteLine("Only image, sound and model plugin can be loaded. For core plugins use GetScriptPlugin, GetUIPlugin and GetSoundPlugin functions");
		return;
	}
	m_plugins.push_back(plugin);
}

void PluginManager::LoadFolder(const Path& folder, bool recursive)
{
	std::vector<Path> files = GetFiles(folder, make_path(L"*.dll"), recursive);
	for (size_t i = 0; i < files.size(); ++i)
	{
		LoadPlugin(files[i]);
	}
}
}