#include "../IPlugin.h"
#include "../Typedefs.h"

class Plugin : public wargameEngine::IPlugin
{
public:
	Plugin(const wargameEngine::Path& str);
	~Plugin();

	void* GetFunction(std::string const& name) override;

private:
	void* m_handle;
};