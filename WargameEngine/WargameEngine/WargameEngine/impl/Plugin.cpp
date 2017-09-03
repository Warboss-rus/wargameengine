#include "Plugin.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif
#include "../LogWriter.h"
#include "../Utils.h"

using namespace wargameEngine;

std::string GetErrorString()
{
#ifdef WIN32
	return std::to_string(GetLastError());
#else
	return dlerror();
#endif
}

Plugin::Plugin(const Path& str)
{
#ifdef WIN32
	m_handle = LoadLibraryW(str.c_str());
#else
	m_handle = dlopen(str.c_str(), RTLD_NOW);
#endif
	if (!m_handle)
	{
		throw std::runtime_error("Error loading plugin '" + str.string() + "'. " + GetErrorString());
	}
}

Plugin::~Plugin()
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

void* Plugin::GetFunction(std::string const& name)
{
	if (!m_handle)
		return nullptr;
#ifdef WIN32
	return (void*)::GetProcAddress((HMODULE)m_handle, name.c_str());
#else
	return ::dlsym(m_handle, name.c_str());
#endif
}