#include "ICommand.h"
#include <string>
#include <memory>

namespace wargameEngine
{
namespace model
{
class IObject;
class IModel;
}

namespace controller
{
class CommandChangeGlobalProperty : public ICommand
{
public:
	CommandChangeGlobalProperty(std::wstring const& key, std::wstring const& value, model::IModel& model);
	CommandChangeGlobalProperty(IReadMemoryStream & stream, model::IModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::wstring m_key;
	std::wstring m_oldValue;
	std::wstring m_newValue;
	model::IModel& m_model;
};
}
}