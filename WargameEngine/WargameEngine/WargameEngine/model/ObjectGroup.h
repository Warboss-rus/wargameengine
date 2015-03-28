#include "ObjectInterface.h"
#include <memory>
#include <vector>

class CObjectGroup : public IObject
{
public:
	CObjectGroup();
	std::string GetPathToModel() const override;
	void Move(double x, double y, double z) override;
	void SetCoords(double x, double y, double z) override;
	void SetCoords(CVector3d const& coords) override;
	void Rotate(double rotation) override;
	double GetX() const override;
	double GetY() const override;
	double GetZ() const override;
	CVector3d GetCoords() const override;
	double GetRotation() const override;
	std::set<std::string> const& GetHiddenMeshes() const override;
	void HideMesh(std::string const& meshName) override;
	void ShowMesh(std::string const& meshName) override;
	void AddChildren(std::shared_ptr<IObject> object);
	void RemoveChildren(std::shared_ptr<IObject> object);
	bool ContainsChildren(std::shared_ptr<IObject> object) const;
	size_t GetCount() const;
	std::shared_ptr<IObject> GetChild(size_t index);
	void DeleteAll();
	void SetCurrent(std::shared_ptr<IObject> object);
	std::shared_ptr<IObject> GetCurrent() const;
	void SetProperty(std::string const& key, std::string const& value) override;
	std::string const GetProperty(std::string const& key) const override;
	bool IsSelectable() const override;
	void SetSelectable(bool selectable) override;
	void SetMovementLimiter(IMoveLimiter * limiter) override;
	std::map<std::string, std::string> const& GetAllProperties() const override;
	bool CastsShadow() const override;
	void PlayAnimation(std::string const& animation, sAnimation::eLoopMode loop, float speed) override;
	std::string GetAnimation() const override;
	float GetAnimationTime() const override;
	void AddSecondaryModel(std::string const& model) override;
	void RemoveSecondaryModel(std::string const& model) override;
	unsigned int GetSecondaryModelsCount() const override;
	std::string GetSecondaryModel(unsigned int index) const override;
	sAnimation::eLoopMode GetAnimationLoop() const override;
	float GetAnimationSpeed() const override;
	void GoTo(CVector3d const& coords, double speed, std::string const& animation, float animationSpeed) override;
	void Update() override;
	std::vector<sTeamColor> const& GetTeamColor() const override;
	void ApplyTeamColor(std::string const& suffix, unsigned char r, unsigned char g, unsigned char b) override;
	void ReplaceTexture(std::string const& oldTexture, std::string const& newTexture) override;
	std::map<std::string, std::string> const& GetReplaceTextures() const override;
private:
	std::vector<std::shared_ptr<IObject>> m_children;
	size_t m_current;
	const std::set<std::string> empty;
};