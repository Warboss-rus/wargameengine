#pragma once
#include "../view/ISoundPlayer.h"
#include <fmod.hpp>
#include <map>

class CSoundPlayerFMod : public wargameEngine::view::ISoundPlayer
{
public:
	virtual void Init() override;
	virtual void Play(std::wstring const& channel, const wargameEngine::Path& file, float volume = 1.0f) override;
	virtual void PlaySoundPosition(std::wstring const& channel, const wargameEngine::Path& file, CVector3f const& position, float volume = 1.0f) override;
	virtual void PlaySoundPlaylist(std::wstring const& channel, const std::vector<wargameEngine::Path>& files, float volume = 1.0f, bool shuffle = false, bool repeat = false) override;
	virtual void SetListenerPosition(CVector3f const& position, CVector3f const& center) override;
	virtual void PauseChannel(std::wstring const& name, bool pause) override;
	virtual void StopChannel(std::wstring const& name) override;
	virtual void Update() override;
	~CSoundPlayerFMod();


private:
	struct sPlaylist
	{
		FMOD::ChannelGroup* group;
		std::vector<FMOD::Sound*> sounds;
		bool repeat;
		bool shuffle;
	};
	void PlayList(sPlaylist & list);

	FMOD::System* m_system;
	std::map<wargameEngine::Path, FMOD::Sound*> m_sounds;
	std::map<wargameEngine::Path, FMOD::Sound*> m_sounds3d;
	std::map<std::wstring, sPlaylist> m_playlists;
	std::map<std::wstring, FMOD::Channel*> m_channels;
};