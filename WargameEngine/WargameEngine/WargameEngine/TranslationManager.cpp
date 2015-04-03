#include "TranslationManager.h"
#include <fstream>

void CTranslationManager::LoadFile(std::string const& path)
{
	std::wifstream iFile(path, std::ios::binary);
	while(iFile.good())
	{
		std::wstring str;
		std::getline(iFile, str);
		auto tab = str.find(L"\t");
		std::wstring wkey = str.substr(0, tab);
		std::string key(wkey.begin(), wkey.end());
		m_dictionary[key] = str.substr(tab + 1);
	}
	iFile.close();
}

std::wstring CTranslationManager::GetTranslation(std::string const& key, std::vector<std::string> const& insertValues)
{
	auto it = m_dictionary.find(key);
	if (it == m_dictionary.end())
	{
		std::wstring result;
		result.assign(key.begin(), key.end());
		return result;
	}
	else
	{
		std::wstring result = it->second;
		for (size_t i = 0; i < insertValues.size(); ++i)
		{
			std::wstring strToReplace = L"{" + std::to_wstring(i) + L"}";
			result.replace(result.find(strToReplace), strToReplace.size(), GetTranslation(insertValues[i]));
		}
		return result;
	}
}

void CTranslationManager::Reset()
{
	m_dictionary.clear();
}