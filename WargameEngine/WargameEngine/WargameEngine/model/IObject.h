#pragma once
#include "IBaseObject.h"
#include "Animation.h"
#include "TeamColor.h"
#include <chrono>
#include <map>
#include <set>
#include <string>
#include <vector>

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
	virtual std::map<std::wstring, std::wstring> const& GetAllProperties() const = 0;
	virtual void PlayAnimation(std::string const& animation, AnimationLoop loop = AnimationLoop::NonLooping, float speed = 1.0f) = 0;
	virtual std::string GetAnimation() const = 0;
	virtual float GetAnimationTime() const = 0;
	virtual void AddSecondaryModel(const Path& model) = 0;
	virtual void RemoveSecondaryModel(const Path& model) = 0;
	virtual size_t GetSecondaryModelsCount() const = 0;
	virtual Path GetSecondaryModel(size_t index) const = 0;
	virtual AnimationLoop GetAnimationLoop() const = 0;
	virtual float GetAnimationSpeed() const = 0;
	virtual void Update(std::chrono::microseconds timeSinceLastUpdate) = 0;
	virtual std::vector<sTeamColor> const& GetTeamColor() const = 0;
	virtual void ApplyTeamColor(std::wstring const& suffix, unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void ReplaceTexture(const Path& oldTexture, const Path& newTexture) = 0;
	virtual std::map<Path, Path> const& GetReplaceTextures() const = 0;
	virtual bool IsGroup() const = 0;
};