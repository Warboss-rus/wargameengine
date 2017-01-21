#include "ObjectInterface.h"
#include <memory>
#include <vector>

class IGameModel;

class CObjectGroup : public IObject
{
public:
	CObjectGroup(IGameModel & model);
	std::wstring GetPathToModel() const override;
	void Move(float dx, float dy, float dz) override;
	void SetCoords(float x, float y, float z) override;
	void SetCoords(CVector3f const& coords) override;
	void Rotate(float rotation) override;
	void SetRotation(float rotation) override;
	float GetX() const override;
	float GetY() const override;
	float GetZ() const override;
	CVector3f GetCoords() const override;
	float GetRotation() const override;
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
	void SetProperty(std::wstring const& key, std::wstring const& value) override;
	std::wstring const GetProperty(std::wstring const& key) const override;
	bool IsSelectable() const override;
	void SetSelectable(bool selectable) override;
	void SetMovementLimiter(IMoveLimiter * limiter) override;
	std::map<std::wstring, std::wstring> const& GetAllProperties() const override;
	bool CastsShadow() const override;
	void PlayAnimation(std::string const& animation, eAnimationLoopMode loop, float speed) override;
	std::string GetAnimation() const override;
	float GetAnimationTime() const override;
	void AddSecondaryModel(std::wstring const& model) override;
	void RemoveSecondaryModel(std::wstring const& model) override;
	size_t GetSecondaryModelsCount() const override;
	std::wstring GetSecondaryModel(size_t index) const override;
	eAnimationLoopMode GetAnimationLoop() const override;
	float GetAnimationSpeed() const override;
	void Update(long long timeSinceLastUpdate) override;
	std::vector<sTeamColor> const& GetTeamColor() const override;
	void ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b) override;
	void ReplaceTexture(std::wstring const& oldTexture, std::wstring const& newTexture) override;
	std::map<std::wstring, std::wstring> const& GetReplaceTextures() const override;
	virtual bool IsGroup() const override;
	virtual IObject* GetFullObject() override;
private:
	std::vector<std::shared_ptr<IObject>> m_children;
	size_t m_current;
	const std::set<std::string> empty;
	IGameModel & m_model;
};