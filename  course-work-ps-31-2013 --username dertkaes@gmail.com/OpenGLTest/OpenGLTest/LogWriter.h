#include <string>
#include <memory>
#pragma once

class CLogWriter
{
public:
	static void WriteLine(std::string const& line);
private:
	static std::string filename;
};