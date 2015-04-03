#include "SoundPlayer.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <string.h>
#include "LogWriter.h"
#include <future>
#define STREAM_BUFFERS 5
#define WAV_FILE_HEADER_SIZE 44
#define WAV_FILE_NUMCHANNELS_POSITION 22

std::shared_ptr<CSoundPlayer> CSoundPlayer::m_instance;

void StreamThread(std::vector<std::string> const& files, bool shuffle, bool repeat, float volume, bool & stop);

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
		m_instance->Init();
	}
	std::weak_ptr<CSoundPlayer> pView(m_instance);

	return pView;
}
void CSoundPlayer::FreeInstance()
{
	m_instance.reset();
}

void CSoundPlayer::Init()
{
	m_stop = false;
	ALCdevice *dev;
	ALCcontext *ctx;
	dev = alcOpenDevice(NULL);
	if (!dev)
	{
		LogWriter::WriteLine("AL error. Cannot initialize device");
		return;
	}
	ctx = alcCreateContext(dev, NULL);
	if (!ctx)
	{
		LogWriter::WriteLine("AL error. Cannot initialize context");
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
	//alDistanceModel(AL_LINEAR_DISTANCE);
}

CSoundPlayer::~CSoundPlayer()
{
	m_stop = true;
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

void CSoundPlayer::Play(std::string const& file, float volume)
{
	if (m_buffers.find(file) == m_buffers.end())
	{
		ReadWav(file);
	}
	ALuint source;
	alGenSources(1, &source);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSourcef(source, AL_GAIN, volume);
	int error = alGetError();
	if (error != AL_NO_ERROR)
	{
		alDeleteSources(1, &source);
		LogWriter::WriteLine("AL error. Error creating a source. " + GetALError(error));
		return;
	}
	alSourcei(source, AL_BUFFER, m_buffers[file]);
	alSourcePlay(source);
	m_sources.push_back(source);
}

void CSoundPlayer::PlaySoundPosition(std::string const& file, CVector3d const& position, float volume)
{
	if (m_buffers.find(file) == m_buffers.end())
	{
		ReadWav(file);
	}
	ALuint source;
	alGenSources(1, &source);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_FALSE);
	int error = alGetError();
	if (error != AL_NO_ERROR)
	{
		alDeleteSources(1, &source);
		LogWriter::WriteLine("AL error. Error creating a source. " + GetALError(error));
		return;
	}
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
	alSourcef(source, AL_GAIN, volume);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		alDeleteSources(1, &source);
		LogWriter::WriteLine("AL error. Error creating a source. " + GetALError(error));
		return;
	}
	alSourcei(source, AL_BUFFER, m_buffers[file]);
	alSourcePlay(source);
	m_sources.push_back(source);
}

void CSoundPlayer::PlaySoundPlaylist(std::vector<std::string> const& files, float volume, bool shuffle, bool repeat)
{
	m_thread = std::async(std::launch::async, [=] {
		StreamThread(files, shuffle, repeat, volume, m_stop);
	});
}

void CSoundPlayer::ReadWav(std::string const& file)
{
	unsigned int buffer;
	alGenBuffers(1, &buffer);
	int error = alGetError();
	if (error != AL_NO_ERROR)
	{
		alDeleteBuffers(1, &buffer);
		LogWriter::WriteLine("AL error. Error creating a buffer. " + GetALError(error));
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
	memcpy(&numChannels, data + WAV_FILE_NUMCHANNELS_POSITION, sizeof(numChannels));
	memcpy(&frequency, data + 24, sizeof(frequency));
	memcpy(&bitsPerSample, data + 34, sizeof(bitsPerSample));
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
		LogWriter::WriteLine("AL error. Error filling a buffer. " + GetALError(error));
		return;
	}
	delete[] data;
	return;
}

void CSoundPlayer::SetListenerPosition(CVector3d const& position, CVector3d const& center)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);
	float ori[6] = {center.x, center.y, center.z, 0.0f, 1.0f, 0.0f};
	alListenerfv(AL_ORIENTATION, ori);
}

void CSoundPlayer::Update()
{
	for (unsigned int i = 0; i < m_sources.size(); ++i)
	{

	}
}

void StreamThread(std::vector<std::string> const& files, bool shuffle, bool repeat, float volume, bool & stop)
{
	ALuint buffers[STREAM_BUFFERS];
	alGenBuffers(STREAM_BUFFERS, buffers);
	ALuint source;
	alGenSources(1, &source);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSourcei(source, AL_LOOPING, AL_FALSE);
	alSourcef(source, AL_GAIN, volume);
	unsigned int curBuffer = 0;//index of the buffer to be filled
	bool firstRun = true;
	do
	{
		for (unsigned int i = 0; i < files.size(); ++i)
		{
			//open the file and read the header
			FILE * f = fopen(files[i].c_str(), "rb");
			fseek(f, 0L, SEEK_END);
			unsigned int fileSize = ftell(f);
			fseek(f, 0L, SEEK_SET);
			unsigned char * header = new unsigned char[WAV_FILE_HEADER_SIZE];
			fread(header, 1, WAV_FILE_HEADER_SIZE, f);
			unsigned short numChannels, bitsPerSample;
			unsigned int frequency;
			fseek(f, WAV_FILE_NUMCHANNELS_POSITION, SEEK_SET);
			fread(&numChannels, sizeof(numChannels), 1, f);
			fread(&frequency, sizeof(frequency), 1, f);
			fseek(f, 34L, SEEK_SET);
			fread(&bitsPerSample, sizeof(bitsPerSample), 1, f);
			fseek(f, 44L, SEEK_SET);
			delete[] header;
			ALenum format;
			if (numChannels == 1 && bitsPerSample == 8) format = AL_FORMAT_MONO8;
			else if (numChannels == 1 && bitsPerSample == 16) format = AL_FORMAT_MONO16;
			else if (numChannels == 2 && bitsPerSample == 8) format = AL_FORMAT_STEREO8;
			else if (numChannels == 2 && bitsPerSample == 16) format = AL_FORMAT_STEREO16;
			unsigned int pieceSize = frequency * numChannels * bitsPerSample / 8;
			unsigned int bytesRead = 44;
			unsigned char * data = new unsigned char[pieceSize];
			while (bytesRead < fileSize)
			{
				if (stop)
				{
					alDeleteSources(1, &source);
					alDeleteBuffers(STREAM_BUFFERS, buffers);
					return;
				}
				if (bytesRead + pieceSize > fileSize) pieceSize = fileSize - bytesRead;
				ALint processed;
				alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
				if (processed > 0 || firstRun)
				{
					alSourceUnqueueBuffers(source, 1, &buffers[curBuffer]);
					fread(data, 1, pieceSize, f);
					alBufferData(buffers[curBuffer], format, data, pieceSize, frequency);
					alSourceQueueBuffers(source, 1, &buffers[curBuffer]);
					bytesRead += pieceSize;
					curBuffer++;
					if (curBuffer == STREAM_BUFFERS) curBuffer = 0;
					alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
				}
				if (firstRun && curBuffer == 0)
				{
					alSourcePlay(source);
					firstRun = false;
					curBuffer = 0;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			delete[] data;
			fclose(f);
		}
	} while (repeat);
	ALint processed;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
	while (processed < STREAM_BUFFERS && !stop)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
	}
	alDeleteSources(1, &source);
	alDeleteBuffers(STREAM_BUFFERS, buffers);
}