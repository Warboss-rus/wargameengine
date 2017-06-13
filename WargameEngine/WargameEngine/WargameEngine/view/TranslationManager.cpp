#include "TranslationManager.h"
#include "..\Utils.h"
#include <fstream>

namespace wargameEngine
{
namespace view
{
void TranslationManager::LoadFile(const Path& path)
{
	std::wifstream iFile(path, std::ios::binary | std::ios::in);
	while (iFile.good())
	{
		std::wstring str;
		std::getline(iFile, str);
		auto tab = str.find(L"\t");
		std::wstring wkey = str.substr(0, tab);
		std::wstring key(wkey.begin(), wkey.end());
		m_dictionary[key] = str.substr(tab + 1);
	}
	iFile.close();
}

std::wstring TranslationManager::GetTranslation(std::wstring const& key, std::vector<std::wstring> const& insertValues)
{
	auto it = m_dictionary.find(key);
	if (it == m_dictionary.end())
	{
		return key;
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

void TranslationManager::Reset()
{
	m_dictionary.clear();
}
}
}