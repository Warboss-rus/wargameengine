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
class CCommandDeleteObject : public ICommand
{
public:
	CCommandDeleteObject(std::shared_ptr<model::IObject> object, model::IModel& model);
	CCommandDeleteObject(IReadMemoryStream & stream, model::IModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<model::IObject> m_pObject;
	model::IModel& m_model;
};
}
}