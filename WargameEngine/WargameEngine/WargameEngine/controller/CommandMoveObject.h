#include "ICommand.h"
#include <string>
#include <memory>

class IObject;
class IGameModel;

class CCommandMoveObject :
	public ICommand
{
public:
	CCommandMoveObject(std::shared_ptr<IObject> object, float deltaX, float deltaY);
	CCommandMoveObject(IReadMemoryStream & stream, IGameModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<IObject> m_pObject;
	float m_deltaX;
	float m_deltaY;
};

