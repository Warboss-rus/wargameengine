#pragma once
#include <map>
#include <memory>
#include <future>
#include "ISoundPlayer.h"

class CSoundPlayerOpenAl : public ISoundPlayer
{
public:
	virtual void Init() override;
	virtual void Play(std::string const& file, float volume = 1.0f) override;
	virtual void PlaySoundPosition(std::string const& file, CVector3d const& position, float volume = 1.0f) override;
	virtual void PlaySoundPlaylist(std::string const& name, std::vector<std::string> const& files, float volume = 1.0f, bool shuffle = true, bool repeat = false) override;
	virtual void SetListenerPosition(CVector3d const& position, CVector3d const& center) override;
	virtual void PausePlaylist(std::string const& name, bool pause) override;
	virtual void StopPlaylist(std::string const& name) override;
	void Update();
	~CSoundPlayerOpenAl();
private:
	void ReadWav(std::string const& file);
	void * m_device;
	void * m_context;
	std::map<std::string, unsigned int> m_buffers;
	std::vector<unsigned int> m_sources;
	std::future<void> m_thread;
	bool m_stop;
};