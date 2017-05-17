#include "OSSpecific.h"
#include "LogWriter.h"
#ifdef _WINDOWS
#include <Windows.h>

namespace wargameEngine
{
std::vector<Path> GetFiles(const Path& path, const Path& mask, bool recursive)
{
	std::vector<Path> result;
	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind;
	std::vector<Path> dir;
	hFind = FindFirstFileW(((!path.empty()) ? path + L"\\" + mask : mask).c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
		do
		{
			if (wcscmp(FindFileData.cFileName, L".") == 0 || wcscmp(FindFileData.cFileName, L"..") == 0)
			{
				continue;
			}
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				dir.push_back(path + L"\\" + FindFileData.cFileName);
			}
			else
			{
				result.push_back(FindFileData.cFileName);
			}
		} while (FindNextFileW(hFind, &FindFileData));
	FindClose(hFind);
	if (recursive)
	{
		for (size_t i = 0; i < dir.size(); ++i)
		{
			std::vector<Path> temp = GetFiles(dir[i], mask, recursive);
			for (auto j = temp.begin(); j != temp.end(); ++j)
			{
				result.push_back(path + L"\\" + *j);
			}
		}
	}
	return result;
}
}
#else
#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <unistd.h>

namespace wargameEngine
{
bool match(char const* needle, char const* haystack)
{
	for (; *needle != '\0'; ++needle)
	{
		switch (*needle)
		{
		case '?':
			++haystack;
			break;
		case '*':
		{
			size_t max = strlen(haystack);
			if (needle[1] == '\0' || max == 0)
				return true;
			for (size_t i = 0; i < max; i++)
				if (match(needle + 1, haystack + i))
					return true;
			return false;
		}
		default:
			if (*haystack != *needle)
				return false;
			++haystack;
		}
	}
	return *haystack == '\0';
}

std::vector<Path> GetFiles(const Path& path, const Path& mask, bool recursive)
{
	std::vector<Path> result;
	std::vector<Path> dirs;
	DIR* d = opendir(path.c_str());
	struct dirent* dir;
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (!strcmp(dir->d_name, ""))
				continue;
			if (dir->d_name[0] == '.')
				continue;

			if (dir->d_type == DT_DIR)
			{
				dirs.push_back(path + "/" + dir->d_name);
			}
			else
			{
				if (match(mask.c_str(), dir->d_name))
				{
					result.push_back(dir->d_name);
				}
			}
		}
		closedir(d);
	}
	if (recursive)
	{
		for (size_t i = 0; i < dirs.size(); ++i)
		{
			std::vector<Path> temp = GetFiles(dirs[i], mask, recursive);
			for (auto i = temp.begin(); i != temp.end(); ++i)
			{
				result.push_back(path + "/" + *i);
			}
		}
	}
	std::sort(result.begin(), result.end());
	return result;
}
}
#endif
