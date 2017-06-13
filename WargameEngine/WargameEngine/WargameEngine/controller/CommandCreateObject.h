#include "ICommand.h"
#include "../model/IObject.h"
#include <string>
#include <memory>

namespace wargameEngine
{
namespace model
{
class IModel;
}

namespace controller
{
class CCommandCreateObject : public ICommand
{
public:
	CCommandCreateObject(std::shared_ptr<model::IObject> const& object, model::IModel& model);
	CCommandCreateObject(IReadMemoryStream & stream, model::IModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<model::IObject> m_pObject;
	model::IModel& m_model;
};
}
}