#pragma once
#include <memory>

class IObject;
class IBaseObject;

class IGameModel
{
public:
	using ObjectPtr = std::shared_ptr<IObject>;

	virtual void DeleteObjectByPtr(const ObjectPtr& pObject) = 0;
	virtual void AddObject(const ObjectPtr& pObject) = 0;
	virtual std::wstring GetProperty(const std::wstring& key) const = 0;
	virtual void SetProperty(const std::wstring& key, const std::wstring& value) = 0;
	virtual ObjectPtr Get3DObject(const IBaseObject* obj) = 0;
	virtual size_t GetObjectCount() const = 0;
	virtual ObjectPtr Get3DObject(size_t index) = 0;

	virtual ~IGameModel() {}
};