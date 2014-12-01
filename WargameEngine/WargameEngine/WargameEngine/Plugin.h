#include <string>

typedef const char* (*PluginGetTypeFunction)();
typedef void* (*PluginGetClassFunction)();

class CPlugin
{
public:
	CPlugin(std::string const& str);
	~CPlugin();
	void * GetFunction(std::string const& name);
private:
	void * m_handle;
};