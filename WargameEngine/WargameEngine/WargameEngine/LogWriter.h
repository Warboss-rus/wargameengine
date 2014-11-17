#include <string>
#include <memory>
#pragma once

class LogWriter
{
public:
	static void WriteLine(std::string const& line);
private:
	static std::string filename;
};