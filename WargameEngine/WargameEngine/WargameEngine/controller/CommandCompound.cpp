#include "CommandCompound.h"

void CCommandCompound::Execute()
{
	for(auto i = m_children.begin(); i != m_children.end(); ++i)
	{
		(*i)->Execute();
	}
}

void CCommandCompound::Rollback()
{
	for(auto i = m_children.end(); i != m_children.begin(); --i)
	{
		(*i)->Execute();
	}
}

void CCommandCompound::AddChild(ICommand* child)
{ 
	m_children.push_back(std::shared_ptr<ICommand>(child)); 
}

ICommand* CCommandCompound::GetChild(size_t index) 
{ 
	return (m_children[index]).get(); 
}

size_t CCommandCompound::GetChildrenCount() const
{
	return m_children.size();
}