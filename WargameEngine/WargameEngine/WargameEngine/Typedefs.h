#pragma once
#include <filesystem>

namespace wargameEngine
{
using Path = std::experimental::filesystem::path;
}

namespace std
{
template<> struct hash<wargameEngine::Path>
{
	size_t operator()(const wargameEngine::Path& p) const
	{
		return std::experimental::filesystem::hash_value(p);
	}
};
}