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
class CCommandMoveObject :
	public ICommand
{
public:
	CCommandMoveObject(std::shared_ptr<model::IObject> object, float deltaX, float deltaY);
	CCommandMoveObject(IReadMemoryStream & stream, model::IModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<model::IObject> m_pObject;
	float m_deltaX;
	float m_deltaY;
};
}
}