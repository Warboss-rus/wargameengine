#include "Plugin.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif
#include "LogWriter.h"
#include "Utils.h"

std::string GetErrorString()
{
#ifdef WIN32
	return std::to_string(GetLastError());
#else
	return dlerror();
#endif
}

CPlugin::CPlugin(const Path& str)
{
#ifdef WIN32
	m_handle = LoadLibraryW(str.c_str());
#else
	m_handle = dlopen(str.c_str(), RTLD_NOW);
#endif
	if (!m_handle)
	{
		LogWriter::WriteLine("Error loading plugin '" + to_string(str) + "'. " + GetErrorString());
	}
}

CPlugin::~CPlugin()
{
	if (m_handle)
	{
#ifdef WIN32
		::FreeLibrary((HMODULE)m_handle);
#else
		::dlclose(m_handle);
#endif
	}
}

void * CPlugin::GetFunction(std::string const& name)
{
	if (!m_handle) return nullptr;
#ifdef WIN32
	return ::GetProcAddress((HMODULE)m_handle, name.c_str());
#else
	return ::dlsym(m_handle, name.c_str());
#endif
}