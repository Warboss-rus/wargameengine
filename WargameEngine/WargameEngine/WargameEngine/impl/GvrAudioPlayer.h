#pragma once
#include "../view/ISoundPlayer.h"
#include "vr/gvr/capi/include/gvr_audio.h"
#include <map>

using wargameEngine::Path;

class CGvrAudioPlayer : public wargameEngine::view::ISoundPlayer
{
public:
	CGvrAudioPlayer(std::unique_ptr<gvr::AudioApi> gvr_audio_api);

	void Init() override;
	void Play(const std::wstring& channel, const Path& file, float volume = 1.0f) override;
	void PlaySoundPosition(const std::wstring& channel, const Path& file, const CVector3f& position, float volume = 1.0f) override;
	void PlaySoundPlaylist(const std::wstring& channel, const std::vector<Path>& files, float volume = 1.0f, bool shuffle = false, bool repeat = false) override;
	void SetListenerPosition(const CVector3f& position, const CVector3f& center) override;
	void PauseChannel(const std::wstring& name, bool pause) override;
	void StopChannel(const std::wstring& name) override;
	void Update() override;
private:
	std::unique_ptr<gvr::AudioApi> m_gvr_audio_api;
	std::map<std::wstring, gvr::AudioSourceId> m_channels;
};