#include<string>
#include<vector>

std::vector<std::string> GetFiles(std::string const& path, std::string const& mask, bool recursive);
void ShowMessageBox(std::string const& text, std::string const& caption);