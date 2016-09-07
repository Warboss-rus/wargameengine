#include "Utils.h"
#include <string>
#include <vector>
#include <chrono>
#include <locale>

#include <fstream>
#include <sstream>
#include <iomanip>
#ifdef TO_STRING_HACK
#include <stdlib.h>
std::wstring Utf8ToWstring(std::string const& str)
{
	std::wstring result;
	result.resize(str.size());
	mbstowcs(&result[0], str.c_str(), str.size());
	return result;
}

std::string WStringToUtf8(std::wstring const& str)
{
	std::string result;
	result.resize(str.size());
	wcstombs(&result[0], str.c_str(), str.size());
	return result;
}
#else
#include <codecvt>
std::wstring Utf8ToWstring(std::string const& str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
}

std::string WStringToUtf8(std::wstring const& str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);
}
#endif

void OpenFile(std::wifstream & stream, std::wstring const& path, std::ios::openmode mode)
{
#ifdef _WINDOWS
	stream.open(path, mode);
#else
	stream.open(WStringToUtf8(path), mode);
#endif
}

void OpenFile(std::ifstream & stream, std::wstring const& path, std::ios::openmode mode /*= 0*/)
{
#ifdef _WINDOWS
	stream.open(path, mode);
#else
	stream.open(WStringToUtf8(path), mode);
#endif
}

std::vector<char> ReadFile(std::wstring const& path)
{
	std::vector<char> result;
	std::ifstream file;
	OpenFile(file, path, std::ios::binary | std::ios::in);
	if (!file) return result;
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	result.resize(static_cast<size_t>(size));
	file.seekg(0, std::ios::beg);
	file.read(result.data(), size);
	return result;
}

void WriteFile(std::wstring const& path, const char* data, size_t size)
{
	std::ofstream file(WStringToUtf8(path), std::ios::binary | std::ios::out);
	file.write(data, size);
}

void WriteFile(std::wstring const& path, std::vector<char> const& data)
{
	WriteFile(path, data.data(), data.size());
}

long long GetCurrentTimeLL()
{
	using namespace std::chrono;
	return time_point_cast<milliseconds>(high_resolution_clock::now()).time_since_epoch().count();
}

std::wstring ToWstring(double value, size_t precision /*= 0*/)
{
	std::wostringstream out;
	out << std::setprecision(precision) << std::fixed << value;
	return out.str();
}
