#pragma once
#include <vector>
class ICommand
{
public:
	virtual ~ICommand(){}
	virtual void Execute() = 0;
	virtual void Rollback() = 0;
	virtual std::vector<char> Serialize() const = 0;
};