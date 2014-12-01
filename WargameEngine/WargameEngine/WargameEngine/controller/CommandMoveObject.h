#include "ICommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandMoveObject :
	public ICommand
{
public:
	CCommandMoveObject(std::shared_ptr<IObject> object, double deltaX, double deltaY);
	void Execute();
	void Rollback();
	std::vector<char> Serialize() const;
private:
	std::shared_ptr<IObject> m_pObject;
	double m_deltaX;
	double m_deltaY;
};

