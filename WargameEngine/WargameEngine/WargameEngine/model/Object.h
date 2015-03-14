#pragma once
#include "ObjectInterface.h"
#include <vector>
#include <memory>

class CObject : public IObject
{
public:
	CObject(std::string const& model, double x, double y, double rotation, bool hasShadow = true);
	std::string GetPathToModel() const { return m_model; }
	void Move(double x, double y, double z);
	void SetCoords(double x, double y, double z);
	void SetCoords(CVector3d const& coords) { m_coords = coords;}
	void Rotate(double rotation);
	double GetX() const { return m_coords.x; }
	double GetY() const { return m_coords.y; }
	double GetZ() const { return m_coords.z; }
	CVector3d GetCoords() const { return m_coords; }
	double GetRotation() const { return m_rotation; }
	std::set<std::string> const& GetHiddenMeshes() const { return m_hiddenMeshes; }
	void HideMesh(std::string const& meshName);
	void ShowMesh(std::string const& meshName);
	void SetProperty(std::string const& key, std::string const& value);
	std::string const GetProperty(std::string const& key) const;
	bool IsSelectable() const { return m_isSelectable; }
	void SetSelectable(bool selectable) { m_isSelectable = selectable; }
	void SetMovementLimiter(IMoveLimiter * limiter) { m_movelimiter.reset(limiter); }
	std::map<std::string, std::string> const& GetAllProperties() const { return m_properties; }
	bool CastsShadow() const { return m_castsShadow; }
	void PlayAnimation(std::string const& animation, sAnimation::eLoopMode loop, float speed);
	std::string GetAnimation() const;
	float GetAnimationTime() const;
	void AddSecondaryModel(std::string const& model) { m_secondaryModels.push_back(model); }
	void RemoveSecondaryModel(std::string const& model);
	unsigned int GetSecondaryModelsCount() const { return m_secondaryModels.size(); }
	std::string GetSecondaryModel(unsigned int index) const { return m_secondaryModels[index]; }
	sAnimation::eLoopMode GetAnimationLoop() const { return m_animationLoop; }
	float GetAnimationSpeed() const { return m_animationSpeed; }
	void GoTo(CVector3d const& coords, double speed, std::string const& animation, float animationSpeed);
	void Update();
	std::vector<sTeamColor> const& GetTeamColor() const { return m_teamColor; }
	void ApplyTeamColor(std::string const& suffix, unsigned char r, unsigned char g, unsigned char b);
	void ReplaceTexture(std::string const& oldTexture, std::string const& newTexture);
	std::map<std::string, std::string> const& GetReplaceTextures() const { return m_replaceTextures; }
private:
	std::string m_model;
	std::vector<std::string> m_secondaryModels;
	CVector3d m_coords;
	double m_rotation;
	std::set<std::string> m_hiddenMeshes;
	std::map<std::string, std::string> m_properties;
	bool m_isSelectable;
	std::unique_ptr<IMoveLimiter> m_movelimiter;
	bool m_castsShadow;
	std::string m_animation;
	long long m_animationBegin;
	sAnimation::eLoopMode m_animationLoop;
	float m_animationSpeed;
	CVector3d m_goTarget;
	double m_goSpeed;
	long long m_lastUpdateTime;
	std::vector<sTeamColor> m_teamColor;
	std::map<std::string, std::string> m_replaceTextures;
};