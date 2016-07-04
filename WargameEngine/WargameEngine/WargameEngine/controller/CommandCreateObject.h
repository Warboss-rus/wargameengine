#include "ICommand.h"
#include "../model/ObjectInterface.h"
#include <string>
#include <memory>

class IGameModel;

class CCommandCreateObject : public ICommand
{
public:
	CCommandCreateObject(std::shared_ptr<IObject> const& object, IGameModel& model);
	CCommandCreateObject(IReadMemoryStream & stream, IGameModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<IObject> m_pObject;
	IGameModel& m_model;
};

