#include "ICommand.h"
#include <string>
#include <memory>

class IObject;
class IGameModel;

class CommandChangeGlobalProperty : public ICommand
{
public:
	CommandChangeGlobalProperty(std::string const& key, std::string const& value, IGameModel& model);
	CommandChangeGlobalProperty(IReadMemoryStream & stream, IGameModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::string m_key;
	std::string m_oldValue;
	std::string m_newValue;
	IGameModel& m_model;
};