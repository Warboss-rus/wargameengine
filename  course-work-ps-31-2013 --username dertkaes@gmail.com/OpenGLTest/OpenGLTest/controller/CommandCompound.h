#pragma once
#include "ICommand.h"
#include <string>
#include <memory>
#include <vector>

class CCommandCompound : public ICommand
{
public:
	void AddChildren(ICommand* child);
	ICommand* GetChild(size_t index);
	size_t GetChildrenCount() const;
	void Execute();
	void Rollback();
	~CCommandCompound();
private:
	std::vector<ICommand*> m_children;
};