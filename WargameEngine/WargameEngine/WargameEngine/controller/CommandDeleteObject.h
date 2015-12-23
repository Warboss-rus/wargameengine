#include "ICommand.h"
#include <string>
#include <memory>

class IObject;
class IGameModel;

class CCommandDeleteObject : public ICommand
{
public:
	CCommandDeleteObject(std::shared_ptr<IObject> object, IGameModel& model);
	void Execute();
	void Rollback();
	std::vector<char> Serialize() const;
private:
	std::shared_ptr<IObject> m_pObject;
	IGameModel& m_model;
};

