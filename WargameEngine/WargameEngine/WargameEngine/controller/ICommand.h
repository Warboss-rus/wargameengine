#pragma once
#include <vector>

class IReadMemoryStream;
class IWriteMemoryStream;

class ICommand
{
public:
	virtual ~ICommand(){}
	virtual void Execute() = 0;
	virtual void Rollback() = 0;
	virtual void Serialize(IWriteMemoryStream & stream) const = 0;
};