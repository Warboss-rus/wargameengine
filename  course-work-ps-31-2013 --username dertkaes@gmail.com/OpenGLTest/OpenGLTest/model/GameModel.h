#pragma once
#include <memory>
#include "../ObjectInterface.h"
#include <vector>
#include <string>

class CGameModel
{
private:
	static const int NULL_INDEX = -1;

	std::vector<std::shared_ptr<IObject>> m_objects;
	long m_selectedObjectIndex;
	static std::shared_ptr<CGameModel> m_instanse;

	CGameModel(void):m_selectedObjectIndex(NULL_INDEX){};
	CGameModel(CGameModel const&){};
	IObject* Get3DObjectIntersectingRay(double begin[3], double end[3]);
public:
	~CGameModel(void);
	static std::weak_ptr<CGameModel> GetIntanse();
	unsigned long GetObjectCount() const;
	std::shared_ptr<const IObject> Get3DObject(unsigned long number) const;
	void AddObject(std::shared_ptr<IObject> pObject);
	void DeleteObjectByPtr(std::shared_ptr<IObject> pObject);
	void SelectObjectByIndex(long index);
	std::shared_ptr<const IObject> GetSelectedObject() const;
	std::shared_ptr<IObject> GetSelectedObject();
};

