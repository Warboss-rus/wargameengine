#include <string>

typedef const char* (*PluginGetTypeFunction)();
typedef void* (*PluginGetClassFunction)();

class CPlugin
{
public:
	CPlugin(std::wstring const& str);
	~CPlugin();
	void * GetFunction(std::string const& name);
private:
	void * m_handle;
};