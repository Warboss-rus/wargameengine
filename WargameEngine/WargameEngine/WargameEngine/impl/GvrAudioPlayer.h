#pragma once
#include "../view/ISoundPlayer.h"
#include "vr/gvr/capi/include/gvr_audio.h"
#include <map>

class CGvrAudioPlayer : public ISoundPlayer
{
public:
	CGvrAudioPlayer(std::unique_ptr<gvr::AudioApi> gvr_audio_api);

	virtual void Init() override;
	virtual void Play(std::wstring const& channel, std::wstring const& file, float volume = 1.0f) override;
	virtual void PlaySoundPosition(std::wstring const& channel, std::wstring const& file, CVector3f const& position, float volume = 1.0f) override;
	virtual void PlaySoundPlaylist(std::wstring const& channel, std::vector<std::wstring> const& files, float volume = 1.0f, bool shuffle = false, bool repeat = false) override;
	virtual void SetListenerPosition(CVector3f const& position, CVector3f const& center) override;
	virtual void PauseChannel(std::wstring const& name, bool pause) override;
	virtual void StopChannel(std::wstring const& name) override;
	virtual void Update() override;
private:
	std::unique_ptr<gvr::AudioApi> m_gvr_audio_api;
	std::map<std::wstring, gvr::AudioSourceId> m_channels;
};