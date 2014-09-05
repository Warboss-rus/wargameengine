#pragma once
#include "icommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandCreateObject : public ICommand
{
public:
	CCommandCreateObject(std::shared_ptr<IObject> object);
	void Execute();
	void Rollback();
	std::vector<char> Serialize() const;
private:
	std::shared_ptr<IObject> m_pObject;
};

