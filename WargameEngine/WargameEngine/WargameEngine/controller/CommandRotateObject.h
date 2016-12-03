#include "ICommand.h"
#include <string>
#include <memory>

class IObject;
class IGameModel;

class CCommandRotateObject :
	public ICommand
{
public:
	CCommandRotateObject(std::shared_ptr<IObject> object, float deltaRotation);
	CCommandRotateObject(IReadMemoryStream & stream, IGameModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<IObject> m_pObject;
	float m_deltaRotation;
};

