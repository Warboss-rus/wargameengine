#include "GvrAudioPlayer.h"
#include "../Utils.h"

CGvrAudioPlayer::CGvrAudioPlayer(std::unique_ptr<gvr::AudioApi> gvr_audio_api)
	: m_gvr_audio_api(std::move(gvr_audio_api))
{

}

void CGvrAudioPlayer::Init()
{
}

void CGvrAudioPlayer::Play(std::wstring const& channel, const Path& file, float volume /*= 1.0f*/)
{
	StopChannel(channel);
	m_gvr_audio_api->PreloadSoundfile(to_string(file));
	auto sound = m_gvr_audio_api->CreateSoundObject(to_string(file));
	m_gvr_audio_api->PlaySound(sound, false);
	m_gvr_audio_api->SetSoundVolume(sound, volume);
	m_channels.emplace(std::make_pair(channel, sound));
}

void CGvrAudioPlayer::PlaySoundPosition(std::wstring const& channel, const Path& file, CVector3f const& position, float volume /*= 1.0f*/)
{
	StopChannel(channel);
	m_gvr_audio_api->PreloadSoundfile(to_string(file));
	auto sound = m_gvr_audio_api->CreateSoundObject(to_string(file));
	m_gvr_audio_api->PlaySound(sound, false);
	m_gvr_audio_api->SetSoundVolume(sound, volume);
	m_gvr_audio_api->SetSoundObjectPosition(sound, position.x, position.y, position.z);
	m_channels.emplace(std::make_pair(channel, sound));
}

void CGvrAudioPlayer::PlaySoundPlaylist(std::wstring const& channel, std::vector<Path> const& files, float volume /*= 1.0f*/, bool shuffle /*= false*/, bool repeat /*= false*/)
{
	StopChannel(channel);
	auto sound = m_gvr_audio_api->CreateSoundObject(to_string(files.front()));
	m_gvr_audio_api->PlaySound(sound, repeat);
	m_gvr_audio_api->SetSoundVolume(sound, volume);
	m_channels.emplace(std::make_pair(channel, sound));
}

void CGvrAudioPlayer::SetListenerPosition(CVector3f const& position, CVector3f const& center)
{
}

void CGvrAudioPlayer::PauseChannel(std::wstring const& name, bool pause)
{
	auto it = m_channels.find(name);
	if (it != m_channels.end())
	{
		if (pause)
		{
			m_gvr_audio_api->PauseSound(it->second);
		}
		else
		{
			m_gvr_audio_api->ResumeSound(it->second);
		}
	}
}

void CGvrAudioPlayer::StopChannel(std::wstring const& name)
{
	auto it = m_channels.find(name);
	if (it != m_channels.end())
	{
		m_gvr_audio_api->StopSound(it->second);
		m_channels.erase(it);
	}
}

void CGvrAudioPlayer::Update()
{
	m_gvr_audio_api->Update();
}
