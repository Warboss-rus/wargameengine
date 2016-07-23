#include "ICommand.h"
#include <string>
#include <memory>

class IObject;
class IGameModel;

class CCommandChangeProperty : public ICommand
{
public:
	CCommandChangeProperty(std::shared_ptr<IObject> object, std::wstring const& key, std::wstring const& value);
	CCommandChangeProperty(IReadMemoryStream & stream, IGameModel& model);
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::shared_ptr<IObject> m_pObject;
	std::wstring m_key;
	std::wstring m_oldValue;
	std::wstring m_newValue;
};