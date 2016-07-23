#include "PluginManager.h"
#include "LogWriter.h"
#include "OSSpecific.h"

void CPluginManager::LoadPlugin(std::wstring const& file)
{
	CPlugin plugin(file);
	PluginGetTypeFunction typeFunc = (PluginGetTypeFunction)plugin.GetFunction("GetType");
	PluginGetClassFunction instanceFunc = (PluginGetClassFunction)plugin.GetFunction("GetClass");
	if (!typeFunc || !instanceFunc)
	{
		LogWriter::WriteLine(L"A plugin " + file + L" don't have a GetType and GetClass functions.");
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

void CPluginManager::LoadFolder(std::wstring const& folder, bool recursive)
{
	std::vector<std::wstring> files = GetFiles(folder, L"*.dll", recursive);
	for (size_t i = 0; i < files.size(); ++i)
	{
		LoadPlugin(files[i]);
	}
}