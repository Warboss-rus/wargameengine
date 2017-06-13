#include "Typedefs.h"

namespace wargameEngine
{
typedef const char* (*PluginGetTypeFunction)();
typedef void* (*PluginGetClassFunction)();

class Plugin
{
public:
	Plugin(const Path& str);
	~Plugin();
	void* GetFunction(std::string const& name);

private:
	void* m_handle;
};
}