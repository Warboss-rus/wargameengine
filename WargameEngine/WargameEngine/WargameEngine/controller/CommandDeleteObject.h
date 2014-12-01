#include "ICommand.h"
#include <string>
#include <memory>
#include "../model/ObjectInterface.h"

class CCommandDeleteObject : public ICommand
{
public:
	CCommandDeleteObject(std::shared_ptr<IObject> object);
	void Execute();
	void Rollback();
	std::vector<char> Serialize() const;
private:
	std::shared_ptr<IObject> m_pObject;
};

