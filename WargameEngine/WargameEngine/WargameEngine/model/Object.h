#pragma once
#include "ObjectInterface.h"
#include <vector>
#include <memory>

class CObject : public IObject
{
public:
	CObject(std::wstring const& model, double x, double y, double z, double rotation, bool hasShadow = true);
	std::wstring GetPathToModel() const override;
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
	void GoTo(CVector3d const& coords, double speed, std::string const& animation, float animationSpeed) override;
	void Update() override;
	std::vector<sTeamColor> const& GetTeamColor() const override;
	void ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b) override;
	void ReplaceTexture(std::wstring const& oldTexture, std::wstring const& newTexture) override;
	std::map<std::wstring, std::wstring> const& GetReplaceTextures() const override;
private:
	std::wstring m_model;
	std::vector<std::wstring> m_secondaryModels;
	CVector3d m_coords;
	double m_rotation;
	std::set<std::string> m_hiddenMeshes;
	std::map<std::wstring, std::wstring> m_properties;
	bool m_isSelectable;
	std::unique_ptr<IMoveLimiter> m_movelimiter;
	bool m_castsShadow;
	std::string m_animation;
	long long m_animationBegin;
	eAnimationLoopMode m_animationLoop = eAnimationLoopMode::HOLDEND;
	float m_animationSpeed = 1.0f;
	CVector3d m_goTarget;
	double m_goSpeed;
	long long m_lastUpdateTime;
	std::vector<sTeamColor> m_teamColor;
	std::map<std::wstring, std::wstring> m_replaceTextures;
};