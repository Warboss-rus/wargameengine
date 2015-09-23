#include "OSSpecific.h"
#include "LogWriter.h"
#ifdef _WINDOWS
#include <direct.h>
#include <Windows.h>

void ChangeWorkingDirectory(std::string const& path)
{
	if (_chdir(path.c_str()) != 0)
	{
		LogWriter::WriteLine("Cannot change working directory. Check if " + path + " exists");
	}
}

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

void ShowMessageBox(std::string const& text, std::string const& caption)
{
	MessageBoxA(NULL, text.c_str(), caption.c_str(), 0);
}
#else
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <algorithm>

void ChangeWorkingDirectory(std::string const& path)
{
    chdir(path.c_str());
}

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

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
void ShowMessageBox(std::string const& text, std::string const& caption)
{
	CFUserNotificationRef pDlg = NULL;
	const void* keys[] = { kCFUserNotificationAlertHeaderKey,
		kCFUserNotificationAlertMessageKey };
	const void* vals[] = {
		CFSTR(caption),
		CFSTR(text)
	};

	CFDictionaryRef dict = CFDictionaryCreate(0, keys, vals,
		sizeof(keys) / sizeof(*keys),
		&kCFTypeDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);

	pDlg = CFUserNotificationCreate(kCFAllocatorDefault, 0,
		kCFUserNotificationPlainAlertLevel,
		&nRes, dict);
}
#else
void ShowMessageBox(std::string const& text, std::string const& caption)
{
	LogWriter::WriteLine(caption);
	LogWriter::WriteLine(text);
}
#endif
#endif
