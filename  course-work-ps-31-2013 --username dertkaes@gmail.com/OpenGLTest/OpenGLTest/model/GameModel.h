#pragma once
#include <memory>
#include "../ObjectInterface.h"
#include <vector>
#include <string>

class CGameModel
{
private:
	std::vector<std::shared_ptr<IObject>> m_objects;
	long m_SelectedObject;
	static std::shared_ptr<CGameModel> m_instanse;

	CGameModel(void):m_SelectedObject(-1){};
	CGameModel(CGameModel const&){};
	IObject* Get3DObjectIntersectingRay(double begin[3], double end[3]);
public:
	~CGameModel(void);
	static std::weak_ptr<CGameModel> GetIntanse();
	unsigned long GetObjectCount() const;
	std::shared_ptr<const IObject> Get3DObject(unsigned long number) const;
	void AddObject(std::shared_ptr<IObject> pObject);
	void DeleteSelectedObject();
	void SelectObjectByIndex(long index);
	std::string GetSelectedObjectModel() const;
	std::shared_ptr<const IObject> GetSelectedObject() const;
	void MoveSelectedObjectTo(double x, double y);
};

