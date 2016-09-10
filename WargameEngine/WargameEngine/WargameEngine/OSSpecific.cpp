#include "OSSpecific.h"
#include "LogWriter.h"
#ifdef _WINDOWS
#include <Windows.h>

std::vector<std::wstring> GetFiles(std::wstring const& path, std::wstring const& mask, bool recursive)
{
	std::vector<std::wstring> result;
	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind;
	std::vector<std::wstring> dir;
	hFind=FindFirstFileW(((!path.empty())?path + L"\\" + mask:mask).c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
		do{
			if (wcscmp(FindFileData.cFileName, L".") == 0 || wcscmp(FindFileData.cFileName, L"..") == 0)
			{
				continue;
			}
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
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
			std::vector<std::wstring> temp = GetFiles(dir[i], mask, recursive);
			for (auto j = temp.begin(); j != temp.end(); ++j)
			{
				result.push_back(path + L"\\" + *j);
			}
		}
	}
	return result;
}
#else
#include "Utils.h"
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <algorithm>

std::vector<std::wstring> GetFiles(std::wstring const& path, std::wstring const& mask, bool recursive)
{
    std::vector<std::wstring> result;
    std::vector<std::wstring> dirs;
    DIR *d = opendir(WStringToUtf8(path).c_str());
    struct dirent *dir;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (!strcmp(dir->d_name, "")) continue;
            if (dir->d_name[0] == '.') continue;

            if (dir->d_type == DT_DIR)
            {
                dirs.push_back(path + L"/" + Utf8ToWstring(dir->d_name));
            }
            else
            {
                result.push_back(Utf8ToWstring(dir->d_name));
            }
        }
        closedir(d);
    }
    if (recursive)
    {
        for (size_t i = 0; i < dirs.size(); ++i)
        {
            std::vector<std::wstring> temp = GetFiles(dirs[i], mask, recursive);
            for (auto i = temp.begin(); i != temp.end(); ++i)
            {
                    result.push_back(path + L"/" + *i);
            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}
#endif
