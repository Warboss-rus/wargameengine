#pragma once
#include "../view/ISoundPlayer.h"
#include <SLES/OpenSLES.h>

class CSoundPlayerOpenSLES : public wargameEngine::view::ISoundPlayer
{
public:
	virtual void Init() override;

	void Play(const std::wstring& channel, const wargameEngine::Path& file, float volume = 1.0f) override;
	void PlaySoundPosition(const std::wstring& channel, const wargameEngine::Path& file, CVector3f const& position, float volume = 1.0f) override;
	void PlaySoundPlaylist(const std::wstring& name, const std::vector<wargameEngine::Path>& files, float volume = 1.0f, bool shuffle = false, bool repeat = false) override;
	void SetListenerPosition(const CVector3f& position, const CVector3f& center) override;
	void PauseChannel(const std::wstring& name, bool pause) override;
	void StopChannel(const std::wstring& name) override;
	void Update() override;

private:
	SLObjectItf m_engineObj;
	SLEngineItf m_engine;
	SLObjectItf m_outputMixObj;
};