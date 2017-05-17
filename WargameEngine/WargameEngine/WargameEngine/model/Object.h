#pragma once
#include "BaseObject.h"
#include "IObject.h"
#include <memory>
#include <vector>

namespace wargameEngine
{
namespace model
{
class Object : public BaseObject<IObject>
{
public:
	Object(const Path& model, const CVector3f& position, float rotation, bool hasShadow = true);

	std::set<std::string> const& GetHiddenMeshes() const override;
	void HideMesh(std::string const& meshName) override;
	void ShowMesh(std::string const& meshName) override;
	void SetProperty(std::wstring const& key, std::wstring const& value) override;
	std::wstring const GetProperty(std::wstring const& key) const override;
	bool IsSelectable() const override;
	void SetSelectable(bool selectable) override;
	std::map<std::wstring, std::wstring> const& GetAllProperties() const override;
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
	std::vector<TeamColor> const& GetTeamColor() const override;
	void ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b) override;
	void ReplaceTexture(const Path& oldTexture, const Path& newTexture) override;
	std::map<Path, Path> const& GetReplaceTextures() const override;
	bool IsGroup() const override;
	IObject* GetFullObject() override;

private:
	std::vector<Path> m_secondaryModels;
	std::set<std::string> m_hiddenMeshes;
	std::map<std::wstring, std::wstring> m_properties;
	bool m_isSelectable;
	std::string m_animation;
	std::chrono::microseconds m_animationTime;
	AnimationLoop m_animationLoop = AnimationLoop::HoldEnd;
	float m_animationSpeed = 1.0f;
	std::vector<TeamColor> m_teamColor;
	std::map<Path, Path> m_replaceTextures;
};
}
}