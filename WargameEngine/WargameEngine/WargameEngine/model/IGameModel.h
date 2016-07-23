#pragma once
#include <memory>

class IObject;

class IGameModel
{
public:
	virtual void DeleteObjectByPtr(std::shared_ptr<IObject> pObject) = 0;
	virtual void AddObject(std::shared_ptr<IObject> pObject) = 0;
	virtual std::wstring GetProperty(std::wstring const& key) const = 0;
	virtual void SetProperty(std::wstring const& key, std::wstring const& value) = 0;
	virtual std::shared_ptr<IObject> Get3DObject(IObject * obj) = 0;
	virtual size_t GetObjectCount() const = 0;
	virtual std::shared_ptr<IObject> Get3DObject(size_t number) = 0;

	virtual ~IGameModel() {}
};