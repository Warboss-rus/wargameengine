#include "ICommand.h"
#include <string>
#include <memory>
#include <vector>

class CCommandCompound : public ICommand
{
public:
	void AddChild(ICommand* child);
	ICommand* GetChild(size_t index);
	size_t GetChildrenCount() const;
	void Execute();
	void Rollback();
	void Serialize(IWriteMemoryStream & stream) const;
private:
	std::vector<std::unique_ptr<ICommand>> m_children;
};