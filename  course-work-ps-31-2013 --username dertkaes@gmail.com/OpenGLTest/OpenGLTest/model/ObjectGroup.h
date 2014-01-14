#include "ObjectInterface.h"
#include <memory>
#include <vector>

class CObjectGroup : public IObject
{
public:
	CObjectGroup():m_current(0) {}
	std::string GetPathToModel() const { return ""; }
	void Move(float x, float y, float z);
	void SetCoords(float x, float y, float z);
	void SetCoords(float3 pos);
	void Rotate(float rotation);
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	CVector3d GetCoords() const { return CVector3d(GetX(), GetY(), GetZ()); }
	float GetRotation() const;
	std::set<std::string> const& GetHiddenMeshes() const;
	void HideMesh(std::string const& meshName);
	void ShowMesh(std::string const& meshName);
	void AddChildren(std::shared_ptr<IObject> object);
	void RemoveChildren(std::shared_ptr<IObject> object);
	bool ContainsChildren(std::shared_ptr<IObject> object) const;
	size_t GetCount() const { return m_children.size(); }
	std::shared_ptr<IObject> GetChild(size_t index);
	void DeleteAll();
	void SetCurrent(std::shared_ptr<IObject> object);
	std::shared_ptr<IObject> GetCurrent() const { return m_children[m_current]; }
	void SetProperty(std::string const& key, std::string const& value);
	std::string const GetProperty(std::string const& key) const;
	bool IsSelectable() const { return true; }
	void SetSelectable(bool selectable);
	void SetMovementLimiter(IMoveLimiter * limiter);
private:
	std::vector<std::shared_ptr<IObject>> m_children;
	size_t m_current;
	const std::set<std::string> empty;
};