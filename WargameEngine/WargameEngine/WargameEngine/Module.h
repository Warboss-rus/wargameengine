#pragma once
#include <string>

struct sModule
{
	std::wstring name;
	int version;
	std::wstring author;
	std::wstring site;
	int minVersion;
	bool playable;
	std::wstring folder;
	std::wstring script;
	std::wstring models;
	std::wstring textures;
	std::wstring shaders;
	void Load(std::wstring const& filename);
};