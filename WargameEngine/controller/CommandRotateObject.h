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
class CCommandRotateObject :
	public ICommand
{
public:
	CCommandRotateObject(std::shared_ptr<model::IObject> object, float deltaRotation);
	CCommandRotateObject(IReadMemoryStream & stream, model::IModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<model::IObject> m_pObject;
	float m_deltaRotation;
};
}
}