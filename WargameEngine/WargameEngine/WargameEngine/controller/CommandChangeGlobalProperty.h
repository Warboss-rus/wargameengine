#include "ICommand.h"
#include <string>
#include <memory>

class IObject;
class IGameModel;

class CommandChangeGlobalProperty : public ICommand
{
public:
	CommandChangeGlobalProperty(std::string const& key, std::string const& value, IGameModel& model);
	void Execute();
	void Rollback();
	std::vector<char> Serialize() const;
private:
	std::string m_key;
	std::string m_oldValue;
	std::string m_newValue;
	IGameModel& m_model;
};