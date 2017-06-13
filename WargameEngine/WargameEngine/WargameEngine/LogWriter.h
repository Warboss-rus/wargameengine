#pragma once
#include <string>

namespace wargameEngine
{
class LogWriter
{
public:
	static void WriteLine(std::string const& line);
	static void WriteLine(std::wstring const& line);

private:
	static std::string filename;
};
}