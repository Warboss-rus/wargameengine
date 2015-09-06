#pragma once
#include <memory>
#include <vector>
#include <string>
#include <map>
#include "ObjectInterface.h"
#include "Projectile.h"
#include "Landscape.h"
#include "Bounding.h"

class CGameModel
{
public:
	static std::weak_ptr<CGameModel> GetInstance();
	static void FreeInstance();
	size_t GetObjectCount() const;
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
	void AddProjectile(CProjectile const& projectile);
	size_t GetProjectileCount() const;
	CProjectile const& GetProjectile(unsigned int index) const;
	void Update();
	void RemoveProjectile(unsigned int index);
	CLandscape & GetLandscape();
	void ResetLandscape(double width, double depth, std::string const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth);
	std::shared_ptr<IBounding> GetBoundingBox(std::string const& path) const;
	void AddBoundingBox(std::string const& path, std::shared_ptr<IBounding> bbox);
private:
	CGameModel(void) {}
	CGameModel(CGameModel const&) = delete;

	std::vector<std::shared_ptr<IObject>> m_objects;
	std::vector<CProjectile> m_projectiles;
	std::shared_ptr<IObject> m_selectedObject;
	static std::shared_ptr<CGameModel> m_instanse;
	std::map<std::string, std::string> m_properties;
	CLandscape m_landscape;
	std::map<std::string, std::shared_ptr<IBounding>> m_boundings;
};