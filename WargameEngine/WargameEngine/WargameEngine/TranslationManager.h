#pragma once
#include <string>
#include <vector>
#include <map>

class CTranslationManager
{
public:
	void LoadFile(std::string const& path);
	std::wstring GetTranslation(std::string const& key, std::vector<std::string> const& insertValues = std::vector<std::string>());
	void Reset();
private:
	std::map<std::string, std::wstring> m_dictionary;
};
