#pragma once
#include "view/ISoundPlayer.h"
#include <SLES/OpenSLES.h>

class CSoundPlayerOpenSLES : public ISoundPlayer
{
public:
	virtual void Init() override;

	virtual void Play(std::wstring const& channel, std::wstring const& file, float volume = 1.0f) override;

	virtual void PlaySoundPosition(std::wstring const& channel, std::wstring const& file, CVector3d const& position, float volume = 1.0f) override;

	virtual void PlaySoundPlaylist(std::wstring const& name, std::vector<std::wstring> const& files, float volume = 1.0f, bool shuffle = false, bool repeat = false) override;

	virtual void SetListenerPosition(CVector3d const& position, CVector3d const& center) override;

	virtual void PauseChannel(std::wstring const& name, bool pause) override;

	virtual void StopChannel(std::wstring const& name) override;

	virtual void Update() override;
private:
	SLObjectItf m_engineObj;
	SLEngineItf m_engine;
	SLObjectItf m_outputMixObj;
};