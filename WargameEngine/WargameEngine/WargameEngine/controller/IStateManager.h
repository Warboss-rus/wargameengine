#pragma once
#include <vector>

class IWriteMemoryStream;
class IReadMemoryStream;

class IStateManager
{
public:
	virtual ~IStateManager() {};

	virtual void SerializeState(IWriteMemoryStream & stream, bool hasAdresses = false) const = 0;
	virtual void LoadState(IReadMemoryStream & stream, bool hasAdresses = false) = 0;
};