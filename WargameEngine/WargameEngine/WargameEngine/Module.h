#pragma once
#include "Typedefs.h"

namespace wargameEngine
{
struct Module
{
	Module() = default;
	Module(const Path& filename);
	std::wstring name;
	int version;
	std::wstring author;
	std::wstring site;
	int minVersion;
	bool playable;
	Path folder;
	Path script;
	Path models;
	Path textures;
	Path shaders;
};
}