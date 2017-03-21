#include "SoundPlayerFMod.h"
#include <stdlib.h>
#include "../Utils.h"
#include "../LogWriter.h"

void CSoundPlayerFMod::Init()
{
	if (FMOD::System_Create(&m_system) != FMOD_OK)
	{
		return;
	}
	FMOD_RESULT result = m_system->init(512, FMOD_INIT_NORMAL, NULL);
	if (result != FMOD_OK)
	{
		LogWriter::WriteLine("Cannot init fmod: " + std::to_string(result));
	}
}

void CSoundPlayerFMod::Play(std::wstring const& channelName, std::wstring const& file, float volume /*= 1.0f*/)
{
	if (m_sounds.find(file) == m_sounds.end())
	{
		FMOD::Sound* sound;
		m_system->createSound(WStringToUtf8(file).c_str(), FMOD_2D | FMOD_CREATESAMPLE, NULL, &sound);
		m_sounds[file] = sound;
	}
	FMOD::Channel* channel;
	m_system->playSound(m_sounds[file], 0, false, &channel);
	channel->setVolume(volume);
	if (!channelName.empty())
	{
		if (m_channels.find(channelName) != m_channels.end())
		{
			m_channels[channelName]->stop();
		}
		m_channels[channelName] = channel;
	}
}

FMOD_VECTOR Vector3dToFMODVector(CVector3f const& vec)
{
	return{ vec.x, vec.y, vec.z };
}

void CSoundPlayerFMod::PlaySoundPosition(std::wstring const& channelName, std::wstring const& file, CVector3f const& position, float volume /*= 1.0f*/)
{
	if (m_sounds3d.find(file) == m_sounds3d.end())
	{
		FMOD::Sound* sound;
		m_system->createSound(WStringToUtf8(file).c_str(), FMOD_3D | FMOD_CREATESAMPLE, NULL, &sound);
		m_sounds3d[file] = sound;
	}
	FMOD::Channel* channel;
	m_system->playSound(m_sounds3d[file], 0, true, &channel);
	FMOD_VECTOR pos = Vector3dToFMODVector(position);
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	channel->set3DAttributes(&pos, &vel);
	channel->setVolume(volume);
	channel->setPaused(false);
	if (!channelName.empty())
	{
		if (m_channels.find(channelName) != m_channels.end())
		{
			m_channels[channelName]->stop();
		}
		m_channels[channelName] = channel;
	}
}

std::vector<FMOD::Sound*> Shuffle(std::vector<FMOD::Sound*> list)
{
	std::vector<FMOD::Sound*> result;
	while (!list.empty())
	{
		int index = rand() % list.size();
		result.push_back(list[index]);
		list.erase(list.begin() + index);
	}
	return result;
}

void CSoundPlayerFMod::PlaySoundPlaylist(std::wstring const& name, std::vector<std::wstring> const& files, float volume /*= 1.0f*/, bool shuffle /*= false*/, bool repeat /*= false*/)
{
	StopChannel(name);
	sPlaylist playlist;
	std::string nameStr = WStringToUtf8(name);
	m_system->createChannelGroup(nameStr.c_str(), &playlist.group);
	playlist.group->setVolume(volume);
	playlist.repeat = repeat;
	playlist.shuffle = shuffle;
	for (auto file : files)
	{
		FMOD::Sound* sound;
		m_system->createStream(WStringToUtf8(file).c_str(), FMOD_2D, NULL, &sound);
		playlist.sounds.push_back(sound);
	}
	PlayList(playlist);
	m_playlists[name.c_str()] = std::move(playlist);
}

void CSoundPlayerFMod::SetListenerPosition(CVector3f const& position, CVector3f const& center)
{
	FMOD_VECTOR pos = Vector3dToFMODVector(position);
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR forw = Vector3dToFMODVector(center);
	FMOD_VECTOR up = { 0.0f, 0.0f, 1.0f };
	m_system->set3DListenerAttributes(0, &pos, &vel, &forw, &up);
}

void CSoundPlayerFMod::PlayList(sPlaylist & playlist)
{
	unsigned long long clockStart = 0;
	int outputRate;
	m_system->getSoftwareFormat(&outputRate, 0, 0);
	playlist.group->setPaused(true);
	for (auto sound : playlist.shuffle ? Shuffle(playlist.sounds) : playlist.sounds)
	{
		FMOD::Channel* channel;
		m_system->playSound(sound, playlist.group, true, &channel);

		if (!clockStart)
		{
			channel->getDSPClock(0, &clockStart);
		}
		channel->setDelay(clockStart, 0, false);
		channel->setPaused(false);
		float freq;
		unsigned int slen;
		sound->getLength(&slen, FMOD_TIMEUNIT_PCM);            /* Get the length of the sound in samples. */
		sound->getDefaults(&freq, 0);                          /* Get the default frequency that the sound was recorded at. */
		slen = (unsigned int)((float)slen / freq * outputRate);     /* Convert the length of the sound to 'output samples' for the output timeline. */
		clockStart += slen;                                        /* Place the sound clock start time to this value after the last one. */		
	}
	playlist.group->setPaused(false);
}

void CSoundPlayerFMod::Update()
{
	m_system->update();
	std::vector<std::wstring> playlistsToDelete;
	for (auto& list: m_playlists)//check if playlists are over
	{
		bool playing = false;
		list.second.group->isPlaying(&playing);
		if (!playing)
		{
			if (list.second.repeat)
			{
				PlayList(list.second);
			}
			else
			{
				playlistsToDelete.push_back(list.first);
			}
		}
	}
	for (auto& list : playlistsToDelete)
	{
		StopChannel(list);
	}
}

CSoundPlayerFMod::~CSoundPlayerFMod()
{
	for (auto list : m_playlists)
	{
		list.second.group->release();
		for (auto sound : list.second.sounds)
		{
			sound->release();
		}
	}
	for (auto sound : m_sounds)
	{
		sound.second->release();
	}
	m_system->close();
	m_system->release();
}

void CSoundPlayerFMod::PauseChannel(std::wstring const& name, bool pause)
{
	if (m_playlists.find(name) != m_playlists.end())
	{
		m_playlists[name].group->setPaused(pause);
	}
	if (m_channels.find(name) != m_channels.end())
	{
		m_channels[name]->setPaused(pause);
	}
}

void CSoundPlayerFMod::StopChannel(std::wstring const& name)
{
	auto it = m_playlists.find(name);
	if (it != m_playlists.end())
	{
		it->second.group->release();
		for (auto sound : it->second.sounds)
		{
			sound->release();
		}
		m_playlists.erase(it);
	}
	auto it2 = m_channels.find(name);
	if (it2 != m_channels.end())
	{
		it2->second->stop();
		m_channels.erase(it2);
	}
}
