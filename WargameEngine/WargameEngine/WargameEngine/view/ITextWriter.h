#pragma once
#include <string>

class ITextWriter
{
public:
	virtual ~ITextWriter() {}

	virtual void PrintText(int x, int y, std::string const& font, unsigned int size, std::string const& text, int width = 0, int height = 0) = 0;
	virtual void PrintText(int x, int y, std::string const& font, unsigned int size, std::wstring const& text, int width = 0, int height = 0) = 0;
	virtual int GetStringHeight(std::string const& font, unsigned int size, std::string const& text) = 0;
	virtual int GetStringWidth(std::string const& font, unsigned int size, std::string const& text) = 0;
	virtual int GetStringHeight(std::string const& font, unsigned int size, std::wstring const& text) = 0;
	virtual int GetStringWidth(std::string const& font, unsigned int size, std::wstring const& text) = 0;
};