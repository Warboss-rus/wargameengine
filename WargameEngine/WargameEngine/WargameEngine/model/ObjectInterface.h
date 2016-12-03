#pragma once
#include <string>
#include <set>
#include <map>
#include <vector>
#include "../view/Vector3.h"
#include "Animation.h"
#include "TeamColor.h"
#include "MovementLimiter.h"

class IBaseObject
{
public:
	~IBaseObject() {}
	virtual std::wstring GetPathToModel() const = 0;
	virtual void SetCoords(float x, float y, float z) = 0;
	virtual void SetCoords(CVector3f const& coords) = 0;
	virtual void Move(float dx, float dy, float dz) = 0;
	virtual CVector3f GetCoords() const = 0;
	virtual float GetX() const = 0;
	virtual float GetY() const = 0;
	virtual float GetZ() const = 0;
	virtual void Rotate(float rotation) = 0;
	virtual void SetRotation(float rotation) = 0;
	virtual float GetRotation() const = 0;
	virtual bool CastsShadow() const = 0;
};

class IObject : public IBaseObject
{
public:
	virtual std::set<std::string> const& GetHiddenMeshes() const = 0;
	virtual void HideMesh(std::string const& meshName) = 0;
	virtual void ShowMesh(std::string const& meshName) = 0;
	virtual void SetProperty(std::wstring const& key, std::wstring const& value) = 0;
	virtual std::wstring const GetProperty(std::wstring const& key) const = 0;
	virtual bool IsSelectable() const = 0;
	virtual void SetSelectable(bool selectable) = 0;
	virtual void SetMovementLimiter(IMoveLimiter * limiter) = 0;
	virtual std::map<std::wstring, std::wstring> const& GetAllProperties() const = 0;
	virtual void PlayAnimation(std::string const& animation, eAnimationLoopMode loop = eAnimationLoopMode::NONLOOPING, float speed = 1.0f) = 0;
	virtual std::string GetAnimation() const = 0;
	virtual float GetAnimationTime() const = 0;
	virtual void AddSecondaryModel(std::wstring const& model) = 0;
	virtual void RemoveSecondaryModel(std::wstring const& model) = 0;
	virtual size_t GetSecondaryModelsCount() const = 0;
	virtual std::wstring GetSecondaryModel(size_t index) const = 0;
	virtual eAnimationLoopMode GetAnimationLoop() const = 0;
	virtual float GetAnimationSpeed() const = 0;
	virtual void Update(long long timeSinceLastUpdate) = 0;
	virtual std::vector<sTeamColor> const& GetTeamColor() const = 0;
	virtual void ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void ReplaceTexture(std::wstring const& oldTexture, std::wstring const& newTexture) = 0;
	virtual std::map<std::wstring, std::wstring> const& GetReplaceTextures() const = 0;
	virtual bool IsGroup() const = 0;
};