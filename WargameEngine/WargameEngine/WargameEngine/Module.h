#pragma once
#include "Typedefs.h"

struct sModule
{
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
	void Load(Path const& filename);
};