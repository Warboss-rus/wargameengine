#pragma once
#include <vector>

class IStateManager
{
public:
	virtual ~IStateManager() {};

	virtual std::vector<char> GetState(bool hasAdresses = false) const = 0;
	virtual void SetState(char* data, bool hasAdresses = false) = 0;
};