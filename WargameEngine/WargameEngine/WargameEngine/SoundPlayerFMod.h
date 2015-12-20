#pragma once
#include "view/ISoundPlayer.h"
#include <fmod.hpp>
#include <map>

class CSoundPlayerFMod : public ISoundPlayer
{
public:
	virtual void Init() override;
	virtual void Play(std::string const& file, float volume = 1.0f) override;
	virtual void PlaySoundPosition(std::string const& file, CVector3d const& position, float volume = 1.0f) override;
	virtual void PlaySoundPlaylist(std::string const& name, std::vector<std::string> const& files, float volume = 1.0f, bool shuffle = false, bool repeat = false) override;
	virtual void SetListenerPosition(CVector3d const& position, CVector3d const& center) override;
	virtual void PausePlaylist(std::string const& name, bool pause) override;
	virtual void StopPlaylist(std::string const& name) override;
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
	std::map<std::string, FMOD::Sound*> m_sounds;
	std::map<std::string, FMOD::Sound*> m_sounds3d;
	std::map<std::string, sPlaylist> m_playlists;
};