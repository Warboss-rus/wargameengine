#pragma once
#include <string>

namespace wargameEngine
{
#ifdef _WINDOWS
using Path = std::wstring;
#else
using Path = std::string;
#endif
}