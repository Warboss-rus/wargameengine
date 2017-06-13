#pragma once
#include <string>
#include <vector>
#include <map>
#include "../Typedefs.h"

namespace wargameEngine
{
namespace view
{
class TranslationManager
{
public:
	void LoadFile(const Path& path);
	std::wstring GetTranslation(std::wstring const& key, std::vector<std::wstring> const& insertValues = std::vector<std::wstring>());
	void Reset();
private:
	std::map<std::wstring, std::wstring> m_dictionary;
};
}
}