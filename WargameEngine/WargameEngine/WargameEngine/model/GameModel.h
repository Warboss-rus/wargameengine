#pragma once
#include <memory>
#include <vector>
#include <string>
#include <map>
#include "ObjectInterface.h"
#include "ObjectStatic.h"
#include "Projectile.h"

class CGameModel
{
public:
	static std::weak_ptr<CGameModel> GetInstance();
	static void FreeInstance();
	unsigned long GetObjectCount() const;
	void Clear();
	std::shared_ptr<const IObject> Get3DObject(unsigned long number) const;
	std::shared_ptr<IObject> Get3DObject(unsigned long number);
	std::shared_ptr<IObject> Get3DObject(IObject * obj);
	void AddObject(std::shared_ptr<IObject> pObject);
	void DeleteObjectByPtr(std::shared_ptr<IObject> pObject);
	void SelectObject(std::shared_ptr<IObject> pObject);
	std::shared_ptr<const IObject> GetSelectedObject() const;
	std::shared_ptr<IObject> GetSelectedObject();
	static bool IsGroup(IObject* object);
	void SetProperty(std::string const& key, std::string const& value);
	std::string const GetProperty(std::string const& key);
	std::map<std::string, std::string> const& GetAllProperties() const;
	std::vector<char> GetState(bool hasAdresses = false) const;
	void SetState(char* data, bool hasAdresses = false);
	void AddStaticObject(std::shared_ptr<CStaticObject> object) { m_staticObjects.push_back(object); }
	void AddProjectile(std::shared_ptr<CProjectile> projectile) { m_projectiles.push_back(projectile); }
	unsigned int GetStaticObjectCount() const { return m_staticObjects.size(); }
	std::shared_ptr<CStaticObject> GetStaticObject(unsigned int index) const { return m_staticObjects[index]; }
	unsigned int GetProjectileCount() const { return m_projectiles.size(); }
	std::shared_ptr<CProjectile> GetProjectile(unsigned int index) const { return m_projectiles[index]; }
	void RemoveProjectile(unsigned int index) { m_projectiles.erase(m_projectiles.begin() + index); }
private:
	CGameModel(void) :m_selectedObject(NULL){};
	CGameModel(CGameModel const&){};

	std::vector<std::shared_ptr<IObject>> m_objects;
	std::vector<std::shared_ptr<CStaticObject>> m_staticObjects;
	std::vector<std::shared_ptr<CProjectile>> m_projectiles;
	std::shared_ptr<IObject> m_selectedObject;
	static std::shared_ptr<CGameModel> m_instanse;
	std::map<std::string, std::string> m_properties;
};