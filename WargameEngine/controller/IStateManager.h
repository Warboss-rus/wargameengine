#pragma once
#include <vector>

namespace wargameEngine
{
class IWriteMemoryStream;
class IReadMemoryStream;

namespace controller
{
class IStateManager
{
public:
	virtual ~IStateManager() {};

	virtual void SerializeState(IWriteMemoryStream & stream, bool hasAdresses = false) const = 0;
	virtual void LoadState(IReadMemoryStream & stream, bool hasAdresses = false) = 0;
};
}
}