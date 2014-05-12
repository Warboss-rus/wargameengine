#include "Module.h"
#include <fstream>

std::string sModule::name;
int sModule::version;
std::string sModule::author;
std::string sModule::site;
int sModule::minVersion;
bool sModule::playable;
std::string sModule::folder;
std::string sModule::script;
std::string sModule::models;
std::string sModule::textures;
std::string sModule::shaders;

std::string RemoveSpaces(std::string const& str)
{
	int begin = str.find_first_not_of(' ');
	int end = str.find_last_not_of(' ');
	if (begin == str.npos || end == str.npos) return "";
	return str.substr(begin, end - begin + 1);
}

std::string AddSlash(std::string const& str)
{
	if (!str.empty() && str.back() != '/' && str.back() != '\\')
	{
		return str + '\\';
	}
	return str;
}

void sModule::Load(std::string const& filename)
{
	std::ifstream iFile(filename);
	std::string line;
	std::string key;
	std::string value;
	while (std::getline(iFile, line))
	{
		int equal = line.find('=');
		int comment = line.find(';');
		int end = (comment == line.npos) ? line.size() : comment - 1;
		if (equal == line.npos || (!comment == line.npos && comment < equal)) continue;
		key = RemoveSpaces(line.substr(0, equal));
		value = RemoveSpaces(line.substr(equal + 1, end - equal));
		if (key == "Name") name = value;
		else if (key == "Version") version = atoi(value.c_str());
		else if (key == "Author") author = value;
		else if (key == "Site") site = value;
		else if (key == "Playable") playable = atoi(value.c_str()) != 0;
		else if (key == "Folder") folder = AddSlash(value);
		else if (key == "Script") script = value;
		else if (key == "Models") models = AddSlash(value);
		else if (key == "Textures") textures = AddSlash(value);
		else if (key == "Shaders") shaders = AddSlash(value);
	}

	iFile.close();
}