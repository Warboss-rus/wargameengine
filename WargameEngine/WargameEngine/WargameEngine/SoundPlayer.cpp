#include "SoundPlayer.h"
#include <AL/al.h>
#include <AL/alc.h>
#include "LogWriter.h"

std::shared_ptr<CSoundPlayer> CSoundPlayer::m_instance;

std::string GetALError(ALenum error)
{
	switch (error)
	{
	case(AL_INVALID_NAME) :
	{
		return "Invalid name paramater passed to AL call.";
	}break;
	case(AL_INVALID_ENUM) :
	{
		return "Invalid enum parameter passed to AL call.";
	}break;
	case(AL_INVALID_VALUE) :
	{
		return "Invalid value parameter passed to AL call.";
	}break;
	case(AL_INVALID_OPERATION) :
	{
		return "Illegal AL call.";
	}break;
	case(AL_OUT_OF_MEMORY) :
	{
		return "Not enough memory.";
	}break;
	default :
	{
		return "Unknown error.";
	}break;
	}
}

std::weak_ptr<CSoundPlayer> CSoundPlayer::GetInstance()
{
	if (!m_instance.get())
	{
		m_instance.reset(new CSoundPlayer());
	}
	std::weak_ptr<CSoundPlayer> pView(m_instance);

	return pView;
}
void CSoundPlayer::FreeInstance()
{
	m_instance.reset();
}

CSoundPlayer::CSoundPlayer()
{
	ALCdevice *dev;
	ALCcontext *ctx;
	dev = alcOpenDevice(NULL);
	if (!dev)
	{
		CLogWriter::WriteLine("AL error. Cannot initialize device");
		return;
	}
	ctx = alcCreateContext(dev, NULL);
	if (!ctx)
	{
		CLogWriter::WriteLine("AL error. Cannot initialize context");
		return;
	}
	alcMakeContextCurrent(ctx);
	m_device = dev;
	m_context = ctx;
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };
	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}

CSoundPlayer::~CSoundPlayer()
{
	for (auto i = m_buffers.begin(); i != m_buffers.end(); ++i)
	{
		alDeleteBuffers(1, &i->second);
	}
	if (m_sources.size() > 0)
		alDeleteSources(m_sources.size(), &m_sources[0]);
	alcMakeContextCurrent(NULL);
	alcDestroyContext((ALCcontext *)m_context);
	alcCloseDevice((ALCdevice *)m_device);
}

void CSoundPlayer::PlaySound(std::string const& file)
{
	if (m_buffers.find(file) == m_buffers.end())
	{
		ReadWav(file);
	}
	ALuint source;
	alGenSources(1, &source);
	int error = alGetError();
	if (error != AL_NO_ERROR)
	{
		alDeleteSources(1, &source);
		CLogWriter::WriteLine("AL error. Error creating a source. " + GetALError(error));
		return;
	}
	alSourcei(source, AL_BUFFER, m_buffers[file]);
	alSourcePlay(source);
	m_sources.push_back(source);
}

void CSoundPlayer::ReadWav(std::string const& file)
{
	unsigned int buffer;
	alGenBuffers(1, &buffer);
	int error = alGetError();
	if (error != AL_NO_ERROR)
	{
		alDeleteBuffers(1, &buffer);
		CLogWriter::WriteLine("AL error. Error creating a buffer. " + GetALError(error));
		return;
	}
	FILE * f = fopen(file.c_str(), "rb");
	fseek(f, 0L, SEEK_END);
	unsigned int size = ftell(f);
	fseek(f, 0L, SEEK_SET);
	unsigned char * data = new unsigned char[size];
	fread(data, 1, size, f);
	fclose(f);
	unsigned short numChannels, bitsPerSample;
	unsigned int frequency;
	ALsizei dataSize;
	memcpy(&numChannels, data + 22, 2);
	memcpy(&frequency, data + 24, 4);
	memcpy(&bitsPerSample, data + 34, 2);
	memcpy(&dataSize, data + 0x44, sizeof(ALsizei));
	ALenum format;
	if (numChannels == 1 && bitsPerSample == 8) format = AL_FORMAT_MONO8;
	else if (numChannels == 1 && bitsPerSample == 16) format = AL_FORMAT_MONO16;
	else if (numChannels == 2 && bitsPerSample == 8) format = AL_FORMAT_STEREO8;
	else if (numChannels == 2 && bitsPerSample == 16) format = AL_FORMAT_STEREO16;
	m_buffers[file] = buffer;
	alBufferData(buffer, format, data + 44, size - 44, frequency);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		CLogWriter::WriteLine("AL error. Error filling a buffer. " + GetALError(error));
		return;
	}
	delete[] data;
	return;
}