#pragma once
#include <string>

namespace wargameEngine
{
class IPlugin
{
public:
	virtual ~IPlugin() = default;

	virtual void* GetFunction(std::string const& name) = 0;
};
}