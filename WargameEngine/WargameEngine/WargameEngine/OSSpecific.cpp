#include "OSSpecific.h"
#include "LogWriter.h"
#ifdef _WINDOWS
#include <Windows.h>

std::vector<std::string> GetFiles(std::string const& path, std::string const& mask, bool recursive)
{
	std::vector<std::string> result;
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	std::vector<std::string> dir;
	hFind=FindFirstFileA(((!path.empty())?path + "\\" + mask:mask).c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
		do{
			if(strcmp(FindFileData.cFileName, ".") == 0 || strcmp(FindFileData.cFileName, "..") == 0)
			{
				continue;
			}
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				dir.push_back(path + "\\" + FindFileData.cFileName);
			}
			else
			{
				result.push_back(FindFileData.cFileName);
			}
		} while (FindNextFileA(hFind, &FindFileData));
	FindClose(hFind);
	if (recursive)
	{
		for (size_t i = 0; i < dir.size(); ++i)
		{
			std::vector<std::string> temp = GetFiles(dir[i], mask, recursive);
			for (auto j = temp.begin(); j != temp.end(); ++j)
			{
				result.push_back(path + "\\" + *j);
			}
		}
	}
	return result;
}
#else
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <algorithm>

std::vector<std::string> GetFiles(std::string const& path, std::string const& mask, bool recursive)
{
    std::vector<std::string> result;
    std::vector<std::string> dirs;
    DIR *d = opendir(path.c_str());
    struct dirent *dir;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (!strcmp(dir->d_name, "")) continue;
            if (dir->d_name[0] == '.') continue;

            if (dir->d_type == DT_DIR)
            {
                dirs.push_back(path + "/" + dir->d_name);
            }
            else
            {
                result.push_back(dir->d_name);
            }
        }
        closedir(d);
    }
    if (recursive)
    {
        for (size_t i = 0; i < dirs.size(); ++i)
        {
            std::vector<std::string> temp = GetFiles(dirs[i], mask, recursive);
            for (auto i = temp.begin(); i != temp.end(); ++i)
            {
                    result.push_back(path + "/" + *i);
            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}
#endif
