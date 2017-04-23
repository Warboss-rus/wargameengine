#include "Typedefs.h"

typedef const char* (*PluginGetTypeFunction)();
typedef void* (*PluginGetClassFunction)();

class CPlugin
{
public:
	CPlugin(const Path& str);
	~CPlugin();
	void * GetFunction(std::string const& name);
private:
	void * m_handle;
};