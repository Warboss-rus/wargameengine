#pragma once
#include <string>

struct sModule
{
	std::string name;
	int version;
	std::string author;
	std::string site;
	int minVersion;
	bool playable;
	std::string folder;
	std::string script;
	std::string models;
	std::string textures;
	std::string shaders;
	void Load(std::string const& filename);
};