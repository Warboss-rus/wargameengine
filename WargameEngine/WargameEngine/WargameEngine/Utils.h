#pragma once
#include <string>
#include <vector>
#include <fstream>

std::wstring Utf8ToWstring(std::string const& str);
std::string WStringToUtf8(std::wstring const& str);
std::vector<char> ReadFile(std::wstring const& path);
void WriteFile(std::wstring const& path, const char* data, size_t size);
void WriteFile(std::wstring const& path, std::vector<char> const& data);
long long GetCurrentTimeLL();
std::wstring ToWstring(double value, size_t precision = 0);
void OpenFile(std::wifstream & stream, std::wstring const& path, std::ios::openmode mode = std::ios::in);
void OpenFile(std::ifstream & stream, std::wstring const& path, std::ios::openmode mode = std::ios::in);

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