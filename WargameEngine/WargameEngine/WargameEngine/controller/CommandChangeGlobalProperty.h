#include "ICommand.h"
#include <string>
#include <memory>

class IObject;
class IGameModel;

class CommandChangeGlobalProperty : public ICommand
{
public:
	CommandChangeGlobalProperty(std::wstring const& key, std::wstring const& value, IGameModel& model);
	CommandChangeGlobalProperty(IReadMemoryStream & stream, IGameModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::wstring m_key;
	std::wstring m_oldValue;
	std::wstring m_newValue;
	IGameModel& m_model;
};