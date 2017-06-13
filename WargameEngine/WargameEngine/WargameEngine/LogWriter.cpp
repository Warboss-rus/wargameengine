#include "LogWriter.h"
#include "Utils.h"
#include <fstream>
#include <time.h>

namespace wargameEngine
{
std::string LogWriter::filename;

void LogWriter::WriteLine(std::string const& line)
{
	if (filename.empty())
	{
		time_t t = time(0);
		struct tm* now = localtime(&t);
		char date[30];
		strftime(date, sizeof(date), "%x.%H-%M-%S_log.txt", now);
		filename = /*std::string("..\\") + */ date;
	}
	std::ofstream oFile(filename, std::ofstream::app);
	time_t t = time(0);
	struct tm* now = localtime(&t);
	oFile << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "\t" << line.c_str() << "\n";
	oFile.close();
}

void LogWriter::WriteLine(std::wstring const& line)
{
	WriteLine(WStringToUtf8(line));
}
}