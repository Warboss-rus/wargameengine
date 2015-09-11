#pragma once
#include <string>

struct sModule
{
	static std::string name;
	static int version;
	static std::string author;
	static std::string site;
	static int minVersion;
	static bool playable;
	static std::string folder;
	static std::string script;
	static std::string models;
	static std::string textures;
	static std::string shaders;
	static void Load(std::string const& filename);
};