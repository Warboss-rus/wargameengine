#include "Module.h"
#include "Utils.h"
#include <algorithm>
#include <fstream>
#include <stdlib.h>

namespace
{
std::wstring RemoveSpaces(std::wstring const& str)
{
	size_t begin = str.find_first_not_of(' ');
	size_t end = str.find_last_not_of(' ');
	if (begin == str.npos || end == str.npos)
		return L"";
	return str.substr(begin, end - begin + 1);
}
}

wargameEngine::Module::Module(Path const& filename)
	:Module()
{
	std::wifstream iFile(filename);
	std::wstring line;
	std::wstring key;
	std::wstring value;
	while (std::getline(iFile, line))
	{
		line.erase(std::remove(line.begin(), line.end(), L'\r'));
		size_t equal = line.find('=');
		size_t comment = line.find(';');
		size_t end = (comment == line.npos) ? line.size() : comment - 1;
		if (equal == line.npos || (comment != line.npos && comment < equal))
			continue;
		key = RemoveSpaces(line.substr(0, equal));
		value = RemoveSpaces(line.substr(equal + 1, end - equal));
		if (key == L"Name")
			name = value;
		else if (key == L"Version")
			version = std::stoi(value.c_str());
		else if (key == L"Author")
			author = value;
		else if (key == L"Site")
			site = value;
		else if (key == L"Playable")
			playable = std::stoi(value.c_str()) != 0;
		else if (key == L"Folder")
			folder = filename.parent_path() / value;
		else if (key == L"Script")
			script = value;
		else if (key == L"Models")
			models = value;
		else if (key == L"Textures")
			textures = value;
		else if (key == L"Shaders")
			shaders = value;
	}

	iFile.close();
}