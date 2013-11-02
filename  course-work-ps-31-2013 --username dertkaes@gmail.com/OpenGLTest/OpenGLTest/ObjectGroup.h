#include "ObjectInterface.h"
#include <memory>
#include <vector>

class CObjectGroup : public IObject
{
public:
	std::string GetPathToModel() const { return ""; }
	void Move(double x, double y, double z);
	void Rotate(double rotation);
	double GetX() const;
	double GetY() const;
	double GetZ() const;
	double GetRotation() const;
	std::set<std::string> const& GetHiddenMeshes() const;
	void HideMesh(std::string const& meshName);
	void ShowMesh(std::string const& meshName);
	void AddChildren(std::shared_ptr<IObject> object);
	void RemoveChildren(std::shared_ptr<IObject> object);
	bool ContainsChildren(std::shared_ptr<IObject> object) const;
	size_t GetCount() const { return m_children.size(); }
	std::shared_ptr<IObject> GetChild(size_t index);
	void DeleteAll();
private:
	std::vector<std::shared_ptr<IObject>> m_children;
};