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
class CCommandChangeProperty : public ICommand
{
public:
	CCommandChangeProperty(std::shared_ptr<model::IObject> object, std::wstring const& key, std::wstring const& value);
	CCommandChangeProperty(IReadMemoryStream & stream, model::IModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<model::IObject> m_pObject;
	std::wstring m_key;
	std::wstring m_oldValue;
	std::wstring m_newValue;
};
}
}