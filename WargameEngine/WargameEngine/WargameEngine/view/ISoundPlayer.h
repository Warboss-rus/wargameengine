#pragma once
#include "../Typedefs.h"
#include "Vector3.h"
#include <vector>

class ISoundPlayer
{
public:
	virtual void Init() = 0;
	virtual void Play(const std::wstring& channel, const Path& file, float volume = 1.0f) = 0;
	virtual void PlaySoundPosition(const std::wstring& channel, const Path& file, CVector3f const& position, float volume = 1.0f) = 0;
	virtual void PlaySoundPlaylist(const std::wstring& channel, std::vector<Path> const& files, float volume = 1.0f, bool shuffle = false, bool repeat = false) = 0;
	virtual void SetListenerPosition(CVector3f const& position, CVector3f const& center) = 0;
	virtual void PauseChannel(const std::wstring& name, bool pause) = 0;
	virtual void StopChannel(const std::wstring& name) = 0;
	virtual void Update() = 0;
	virtual ~ISoundPlayer() {}
};