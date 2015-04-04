#include "PluginManager.h"
#include "LogWriter.h"
#include "OSSpecific.h"

void CPluginManager::LoadPlugin(std::string const& file)
{
	CPlugin plugin(file);
	PluginGetTypeFunction typeFunc = (PluginGetTypeFunction)plugin.GetFunction("GetType");
	PluginGetClassFunction instanceFunc = (PluginGetClassFunction)plugin.GetFunction("GetClass");
	if (!typeFunc || !instanceFunc)
	{
		LogWriter::WriteLine("A plugin " + file + " don't have a GetType and GetClass functions.");
		return;
	}
	std::string type = typeFunc();
	void * instance = instanceFunc();
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

void CPluginManager::LoadFolder(std::string const& folder, bool recursive)
{
	std::vector<std::string> files = GetFiles(folder, "*.dll", recursive);
	for (size_t i = 0; i < files.size(); ++i)
	{
		LoadPlugin(files[i]);
	}
}