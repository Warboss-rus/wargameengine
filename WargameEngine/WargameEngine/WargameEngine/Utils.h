#pragma once
#include "Typedefs.h"
#include <vector>
#include <fstream>
#include <unordered_map>

namespace wargameEngine
{
std::wstring Utf8ToWstring(std::string const& str);
std::string WStringToUtf8(std::wstring const& str);
std::vector<char> ReadFile(const Path& path);
void WriteFile(const Path& path, const char* data, size_t size);
void WriteFile(const Path& path, std::vector<char> const& data);
std::wstring ToWstring(double value, size_t precision = 0);
std::wstring ReplaceAll(const std::wstring& text, const std::unordered_map<std::wstring, std::wstring>& replaceMap);
std::string to_string(const Path& path);
std::wstring to_wstring(const Path& path);

#ifdef _WINDOWS
inline Path make_path(const std::wstring& p) { return p; }
inline Path make_path(const std::string& p) { return Utf8ToWstring(p); }
#else
inline Path make_path(const std::wstring& p) { return WStringToUtf8(p); }
inline Path make_path(const std::string& p) { return p; }
#endif
}

#ifdef TO_STRING_HACK
#include <sstream>
#include <stdlib.h>
namespace std
{
	template<class T>
	std::string to_string(T _Val)
	{
		std::stringstream stream;
		stream << _Val;
		return stream.str();
	}

	template<class T>
	std::wstring to_wstring(T _Val)
	{
		std::wstringstream stream;
		stream << _Val;
		return stream.str();
	}

	inline int stoi(const wstring& _Str, size_t */*_Idx*/ = 0, int _Base = 10)
	{
		wchar_t *_Eptr;
		return wcstol(_Str.c_str(), &_Eptr, _Base);
	}
	inline float stof(const string& _Str, size_t */*_Idx*/  = 0 )
	{
		char *_Eptr;
		return strtof(_Str.c_str(), &_Eptr);
	}
}
#endif