#include "OSSpecific.h"
#ifdef _WINDOWS
#include <Windows.h>

void ChangeDir(std::string const& path)
{
	SetCurrentDirectoryA(path.c_str());
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
			for (auto i = temp.begin(); i != temp.end(); ++i)
			{
				result.push_back(path + "\\" + *i);
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
#include <dirent.h> 

void ChangeDir(std::string const& path)
{
	chdir(path.c_str());
}

std::vector<std::string> GetFiles(std::string const& path, std::string const& mask, bool recursive)
{
	std::vector<std::string> result;
	DIR           *d;
	struct dirent *dir;
	d = opendir(path);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG)
			{
				result.push_back(dir->d_name);
			}
		}

		closedir(d);
	}
	return result;
}

void ShowMessageBox(std::string const& text, std::string const& caption)
{
	CLogWriter::WriteLine(caption);
	CLogWriter::WriteLine(text);
}
#endif
