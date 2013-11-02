#pragma once
#include <memory>
#include "../ObjectInterface.h"
#include <vector>
#include <string>

class CGameModel
{
private:
	std::vector<std::shared_ptr<IObject>> m_objects;
	std::shared_ptr<IObject> m_selectedObject;
	static std::shared_ptr<CGameModel> m_instanse;

	CGameModel(void):m_selectedObject(NULL){};
	CGameModel(CGameModel const&){};
public:
	~CGameModel(void);
	static std::weak_ptr<CGameModel> GetIntanse();
	unsigned long GetObjectCount() const;
	std::shared_ptr<const IObject> Get3DObject(unsigned long number) const;
	std::shared_ptr<IObject> Get3DObject(unsigned long number);
	void AddObject(std::shared_ptr<IObject> pObject);
	void DeleteObjectByPtr(std::shared_ptr<IObject> pObject);
	void SelectObject(std::shared_ptr<IObject> pObject);
	std::shared_ptr<const IObject> GetSelectedObject() const;
	std::shared_ptr<IObject> GetSelectedObject();
	static bool IsGroup(std::shared_ptr<IObject> object);
};

