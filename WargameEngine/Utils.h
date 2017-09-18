#pragma once
#include "Typedefs.h"
#include <vector>
#include <unordered_map>

namespace wargameEngine
{
std::wstring Utf8ToWstring(std::string const& str);
std::string WStringToUtf8(std::wstring const& str);
std::vector<char> ReadFile(const Path& path);
void WriteFile(const Path& path, const char* data, size_t size);
void WriteFile(const Path& path, std::vector<char> const& data);
std::wstring ToWstring(double value, size_t precision = 0);
std::wstring ReplaceAll(std::wstring const& text, std::unordered_map<std::wstring, std::wstring> const& replaceMap);
}