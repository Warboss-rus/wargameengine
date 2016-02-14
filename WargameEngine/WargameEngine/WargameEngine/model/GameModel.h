#pragma once
#include <memory>
#include <vector>
#include <string>
#include <map>
#include "IGameModel.h"
#include "IBoundingBoxManager.h"
#include "ObjectInterface.h"
#include "Projectile.h"
#include "Landscape.h"
#include "Bounding.h"

class CGameModel : public IGameModel, public IBoundingBoxManager
{
public:
	CGameModel() = default;
	size_t GetObjectCount() const;
	void Clear();
	std::shared_ptr<const IObject> Get3DObject(unsigned long number) const;
	std::shared_ptr<IObject> Get3DObject(unsigned long number);
	std::shared_ptr<IObject> Get3DObject(IObject * obj);
	virtual void AddObject(std::shared_ptr<IObject> pObject) override;
	virtual void DeleteObjectByPtr(std::shared_ptr<IObject> pObject) override;
	void SelectObject(std::shared_ptr<IObject> pObject);
	std::shared_ptr<const IObject> GetSelectedObject() const;
	std::shared_ptr<IObject> GetSelectedObject();
	static bool IsGroup(IObject* object);
	virtual void SetProperty(std::string const& key, std::string const& value) override;
	virtual std::string GetProperty(std::string const& key) const override;
	std::map<std::string, std::string> const& GetAllProperties() const;
	void AddProjectile(CProjectile const& projectile);
	size_t GetProjectileCount() const;
	CProjectile const& GetProjectile(unsigned int index) const;
	void Update();
	void RemoveProjectile(unsigned int index);
	CLandscape & GetLandscape();
	void ResetLandscape(double width, double depth, std::string const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth);
	std::shared_ptr<IBounding> GetBoundingBox(std::string const& path) const;
	virtual void AddBoundingBox(std::string const& path, std::shared_ptr<IBounding> bbox) override;
private:
	CGameModel(CGameModel const&) = delete;

	std::vector<std::shared_ptr<IObject>> m_objects;
	std::vector<CProjectile> m_projectiles;
	std::shared_ptr<IObject> m_selectedObject;
	std::map<std::string, std::string> m_properties;
	CLandscape m_landscape;
	std::map<std::string, std::shared_ptr<IBounding>> m_boundings;
};