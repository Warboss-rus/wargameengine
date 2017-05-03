#pragma once
#include <string>

#ifdef _WINDOWS
using Path = std::wstring;
#else
using Path = std::string;
#endif