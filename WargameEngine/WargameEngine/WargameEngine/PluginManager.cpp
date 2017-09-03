#include "PluginManager.h"
#include "LogWriter.h"
#include "AsyncFileProvider.h"
#include "Utils.h"

namespace
{
typedef const char* (*PluginGetTypeFunction)();
typedef void* (*PluginGetClassFunction)();
}

namespace wargameEngine
{

PluginManager::PluginManager(const PluginFactory& factory)
	: m_pluginFactory(factory)
{
}

void PluginManager::LoadPlugin(const Path& file)
{
	auto plugin = m_pluginFactory(file);
	PluginGetTypeFunction typeFunc = reinterpret_cast<PluginGetTypeFunction>(plugin->GetFunction("GetType"));
	PluginGetClassFunction instanceFunc = reinterpret_cast<PluginGetClassFunction>(plugin->GetFunction("GetClass"));
	if (!typeFunc || !instanceFunc)
	{
		LogWriter::WriteLine("A plugin " + file.string() + " don't have a GetType and GetClass functions.");
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
	m_plugins.push_back(std::move(plugin));
}

void PluginManager::LoadFolder(const Path& folder, bool recursive)
{
	std::vector<Path> files = GetFiles(folder, L"*.dll", recursive);
	for (size_t i = 0; i < files.size(); ++i)
	{
		LoadPlugin(files[i]);
	}
}
}