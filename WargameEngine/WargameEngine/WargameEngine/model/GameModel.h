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
	virtual size_t GetObjectCount() const override;
	void Clear();
	std::shared_ptr<const IObject> Get3DObject(size_t number) const;
	virtual std::shared_ptr<IObject> Get3DObject(size_t number) override;
	virtual std::shared_ptr<IObject> Get3DObject(IObject * obj) override;
	virtual void AddObject(std::shared_ptr<IObject> pObject) override;
	virtual void DeleteObjectByPtr(std::shared_ptr<IObject> pObject) override;
	void SelectObject(std::shared_ptr<IObject> pObject);
	std::shared_ptr<const IObject> GetSelectedObject() const;
	std::shared_ptr<IObject> GetSelectedObject();
	static bool IsGroup(IObject* object);
	virtual void SetProperty(std::wstring const& key, std::wstring const& value) override;
	virtual std::wstring GetProperty(std::wstring const& key) const override;
	std::map<std::wstring, std::wstring> const& GetAllProperties() const;
	void AddProjectile(CProjectile const& projectile);
	size_t GetProjectileCount() const;
	CProjectile const& GetProjectile(size_t index) const;
	void Update();
	void RemoveProjectile(unsigned int index);
	CLandscape & GetLandscape();
	void ResetLandscape(double width, double depth, std::wstring const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth);
	std::shared_ptr<IBounding> GetBoundingBox(std::wstring const& path) const;
	virtual void AddBoundingBox(std::wstring const& path, std::shared_ptr<IBounding> bbox) override;
private:
	CGameModel(CGameModel const&) = delete;

	std::vector<std::shared_ptr<IObject>> m_objects;
	std::vector<CProjectile> m_projectiles;
	std::shared_ptr<IObject> m_selectedObject;
	std::map<std::wstring, std::wstring> m_properties;
	CLandscape m_landscape;
	std::map<std::wstring, std::shared_ptr<IBounding>> m_boundings;
};