#pragma once
#include <memory>

class IObject;

class IGameModel
{
public:
	virtual void DeleteObjectByPtr(std::shared_ptr<IObject> pObject) = 0;
	virtual void AddObject(std::shared_ptr<IObject> pObject) = 0;
	virtual std::string GetProperty(std::string const& key) const = 0;
	virtual void SetProperty(std::string const& key, std::string const& value) = 0;

	virtual ~IGameModel() {}
};