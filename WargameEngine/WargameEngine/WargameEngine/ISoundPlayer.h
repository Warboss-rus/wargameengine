#pragma once
#include <string>
#include "view/Vector3.h"
#include <vector>

class ISoundPlayer
{
public:
	virtual void Init() = 0;
	virtual void Play(std::string const& file, float volume = 1.0f) = 0;
	virtual void PlaySoundPosition(std::string const& file, CVector3d const& position, float volume = 1.0f) = 0;
	virtual void PlaySoundPlaylist(std::string const& name, std::vector<std::string> const& files, float volume = 1.0f, bool shuffle = false, bool repeat = false) = 0;
	virtual void SetListenerPosition(CVector3d const& position, CVector3d const& center) = 0;
	virtual void PausePlaylist(std::string const& name, bool pause) = 0;
	virtual void StopPlaylist(std::string const& name) = 0;
	virtual void Update() = 0;
	virtual ~ISoundPlayer() {}
};