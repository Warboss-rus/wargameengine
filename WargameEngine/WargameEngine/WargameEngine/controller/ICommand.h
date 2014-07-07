#pragma once
class ICommand
{
public:
	virtual ~ICommand(){}
	virtual void Execute() = 0;
	virtual void Rollback() = 0;
};