#pragma once
#include "icommand.h"
#include <string>
#include <memory>

class IObject;

class CCommandCreateObject :
	public ICommand
{
public:
	CCommandCreateObject(std::string const& model, double x, double y, double rotation);
	void Execute();
	void Rollback();
private:
	std::string m_model;
	double m_x;
	double m_y;
	double m_rotation;
	std::shared_ptr<IObject> m_pObject;
};

