#include "IObject.h"
#include <memory>
#include <vector>

class IGameModel;

class CObjectGroup : public IObject
{
public:
	CObjectGroup(IGameModel & model);
	Path GetPathToModel() const override;
	void Move(float dx, float dy, float dz) override;
	void SetCoords(float x, float y, float z) override;
	void SetCoords(CVector3f const& coords) override;
	void Rotate(float rotation) override;
	void SetRotation(float rotation) override;
	void SetRotations(const CVector3f& rotations) override;
	CVector3f GetRotations() const override;
	float GetX() const override;
	float GetY() const override;
	float GetZ() const override;
	CVector3f GetCoords() const override;
	float GetRotation() const override;
	std::set<std::string> const& GetHiddenMeshes() const override;
	void HideMesh(std::string const& meshName) override;
	void ShowMesh(std::string const& meshName) override;
	void AddChildren(std::shared_ptr<IObject> const& object);
	void RemoveChildren(std::shared_ptr<IObject> const& object);
	bool ContainsChildren(std::shared_ptr<IObject> const& object) const;
	size_t GetCount() const;
	std::shared_ptr<IObject> GetChild(size_t index);
	void DeleteAll();
	void SetCurrent(std::shared_ptr<IObject> const& object);
	std::shared_ptr<IObject> GetCurrent() const;
	void SetProperty(std::wstring const& key, std::wstring const& value) override;
	std::wstring const GetProperty(std::wstring const& key) const override;
	bool IsSelectable() const override;
	void SetSelectable(bool selectable) override;
	void SetMovementLimiter(IMoveLimiter * limiter) override;
	std::map<std::wstring, std::wstring> const& GetAllProperties() const override;
	bool CastsShadow() const override;
	void PlayAnimation(std::string const& animation, AnimationLoop loop, float speed) override;
	std::string GetAnimation() const override;
	float GetAnimationTime() const override;
	void AddSecondaryModel(const Path& model) override;
	void RemoveSecondaryModel(const Path& model) override;
	size_t GetSecondaryModelsCount() const override;
	Path GetSecondaryModel(size_t index) const override;
	AnimationLoop GetAnimationLoop() const override;
	float GetAnimationSpeed() const override;
	void Update(std::chrono::microseconds timeSinceLastUpdate) override;
	std::vector<sTeamColor> const& GetTeamColor() const override;
	void ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b) override;
	void ReplaceTexture(const Path& oldTexture, const Path& newTexture) override;
	std::map<Path, Path> const& GetReplaceTextures() const override;
	virtual bool IsGroup() const override;
	virtual IObject* GetFullObject() override;
	virtual CSignalConnection DoOnCoordsChange(CoordsSignal::Slot const& handler) override;
	virtual CSignalConnection DoOnRotationChange(RotationSignal::Slot const& handler) override;
private:
	std::vector<std::shared_ptr<IObject>> m_children;
	size_t m_current;
	const std::set<std::string> empty;
	IGameModel & m_model;
};