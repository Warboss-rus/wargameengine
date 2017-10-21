#include "LogWriter.h"
#include "Utils.h"
#include <fstream>
#include <time.h>

namespace wargameEngine
{
static Path g_logPath;

void LogWriter::WriteLine(std::string const& line)
{
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm now;
#ifdef _WINDOWS
	localtime_s(&now, &t);
#else
	now = *localtime_r(&t);
#endif
	static std::string filename;
	if (filename.empty())
	{
		char date[30];
		strftime(date, sizeof(date), "%x.%H-%M-%S_log.txt", &now);
		filename = (g_logPath / date).string();
	}
	std::ofstream oFile(filename, std::ofstream::app);
	oFile << now.tm_hour << ":" << now.tm_min << ":" << now.tm_sec << "\t" << line << "\n";
	oFile.close();
}

void LogWriter::WriteLine(std::wstring const& line)
{
	WriteLine(WStringToUtf8(line));
}

void LogWriter::SetLogLocation(Path const& path)
{
	g_logPath = path;
}

}