#include "ObjectInterface.h"
#include <memory>
#include <vector>

class CObjectGroup : public IObject
{
public:
	CObjectGroup():m_current(0) {}
	std::string GetPathToModel() const { return ""; }
	void Move(double x, double y, double z);
	void SetCoords(double x, double y, double z);
	void SetCoords(CVector3d const& coords);
	void Rotate(double rotation);
	double GetX() const;
	double GetY() const;
	double GetZ() const;
	CVector3d GetCoords() const { return CVector3d(GetX(), GetY(), GetZ()); }
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
	void SetCurrent(std::shared_ptr<IObject> object);
	std::shared_ptr<IObject> GetCurrent() const { return m_children[m_current]; }
	void SetProperty(std::string const& key, std::string const& value);
	std::string const GetProperty(std::string const& key) const;
	bool IsSelectable() const { return true; }
	void SetSelectable(bool selectable);
	void SetMovementLimiter(IMoveLimiter * limiter);
	std::map<std::string, std::string> const& GetAllProperties() const { return m_children[0]->GetAllProperties(); }
	bool CastsShadow() const { return true; }
	void PlayAnimation(std::string const& animation, sAnimation::eLoopMode loop, float speed);
	std::string GetAnimation() const { return ""; }
	float GetAnimationTime() const { return 0.0f; }
	void AddSecondaryModel(std::string const& model) {}
	void RemoveSecondaryModel(std::string const& model) {}
	unsigned int GetSecondaryModelsCount() const { return 0; }
	std::string GetSecondaryModel(unsigned int index) const { return ""; }
	sAnimation::eLoopMode GetAnimationLoop() const { return sAnimation::NONLOOPING; }
	float GetAnimationSpeed() const { return 1.0f; }
	void GoTo(CVector3d const& coords, double speed, std::string const& animation, float animationSpeed);
	void Update();
	std::vector<sTeamColor> const& GetTeamColor() const { return std::vector<sTeamColor>(); }
	void ApplyTeamColor(std::string const& suffix, unsigned char r, unsigned char g, unsigned char b);
	void ReplaceTexture(std::string const& oldTexture, std::string const& newTexture);
	std::map<std::string, std::string> const& GetReplaceTextures() const { return std::map<std::string, std::string>(); }
private:
	std::vector<std::shared_ptr<IObject>> m_children;
	size_t m_current;
	const std::set<std::string> empty;
};