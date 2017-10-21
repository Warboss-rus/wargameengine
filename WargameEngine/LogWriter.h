#pragma once
#include <string>
#include "Typedefs.h"

namespace wargameEngine
{
class LogWriter
{
public:
	static void WriteLine(std::string const& line);
	static void WriteLine(std::wstring const& line);
	static void SetLogLocation(Path const& path);
};
}