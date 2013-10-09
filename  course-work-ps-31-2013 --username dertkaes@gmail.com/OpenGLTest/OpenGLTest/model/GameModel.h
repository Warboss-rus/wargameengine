#pragma once
#include <memory>
#include "../ObjectInterface.h"
#include <vector>
#include <string>

class CGameModel
{
private:
	std::vector<IObject *> m_objects;

	static std::shared_ptr<CGameModel> m_instanse;
	CGameModel(void){};
	CGameModel(CGameModel const&){};
public:
	~CGameModel(void);
	static std::weak_ptr<CGameModel> GetIntanse();
	unsigned long GetObjectCount() const;
	IObject* Get3DObject(unsigned long number) const;
	void AddObject(IObject * object);
	void DeleteObject(IObject * object);
};

