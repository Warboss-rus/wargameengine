#pragma once
#include "ObjectInterface.h"
#include <vector>
#include <memory>
#include "ObjectStatic.h"

class CObject : public CBaseObject<IObject>
{
public:
	CObject(std::wstring const& model, CVector3f const& position, float rotation, bool hasShadow = true);
	std::set<std::string> const& GetHiddenMeshes() const override;
	void HideMesh(std::string const& meshName) override;
	void ShowMesh(std::string const& meshName) override;
	void SetProperty(std::wstring const& key, std::wstring const& value) override;
	std::wstring const GetProperty(std::wstring const& key) const override;
	bool IsSelectable() const override;
	void SetSelectable(bool selectable) override;
	void SetMovementLimiter(IMoveLimiter * limiter) override;
	std::map<std::wstring, std::wstring> const& GetAllProperties() const override;
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
	std::vector<std::wstring> m_secondaryModels;
	std::set<std::string> m_hiddenMeshes;
	std::map<std::wstring, std::wstring> m_properties;
	bool m_isSelectable;
	std::unique_ptr<IMoveLimiter> m_movelimiter;
	std::string m_animation;
	long long m_animationTime;
	eAnimationLoopMode m_animationLoop = eAnimationLoopMode::HOLDEND;
	float m_animationSpeed = 1.0f;
	std::vector<sTeamColor> m_teamColor;
	std::map<std::wstring, std::wstring> m_replaceTextures;
};