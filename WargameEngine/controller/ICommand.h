#pragma once
#include <vector>

namespace wargameEngine
{
class IWriteMemoryStream;
class IReadMemoryStream;

namespace controller
{
class ICommand
{
public:
	virtual ~ICommand() {}
	virtual void Execute() = 0;
	virtual void Rollback() = 0;
	virtual void Serialize(IWriteMemoryStream & stream) const = 0;
};
}
}