#include "Icommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandRotateObject :
	public ICommand
{
public:
	CCommandRotateObject(std::shared_ptr<IObject> object, double deltaRotation);
	void Execute();
	void Rollback();
	std::vector<char> Serialize() const;
private:
	std::shared_ptr<IObject> m_pObject;
	double m_deltaRotation;
};

