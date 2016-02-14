#include "ICommand.h"
#include <string>
#include <memory>

class IObject;
class IGameModel;

class CCommandChangeProperty : public ICommand
{
public:
	CCommandChangeProperty(std::shared_ptr<IObject> object, std::string const& key, std::string const& value);
	CCommandChangeProperty(IReadMemoryStream & stream, IGameModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<IObject> m_pObject;
	std::string m_key;
	std::string m_oldValue;
	std::string m_newValue;
};