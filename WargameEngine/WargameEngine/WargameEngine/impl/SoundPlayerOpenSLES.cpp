#include "SoundPlayerOpenSLES.h"
#include <SLES/OpenSLES_Android.h>
#include <android/asset_manager.h>
#include "../LogWriter.h"

void CSoundPlayerOpenSLES::Init()
{
	const SLInterfaceID pIDs[1] = { SL_IID_ENGINE };
	const SLboolean pIDsRequired[1] = { SL_BOOLEAN_TRUE };
	SLresult result = slCreateEngine(&m_engineObj, 0, NULL, 1, pIDs, pIDsRequired);
	if (result != SL_RESULT_SUCCESS) {
		LogWriter::WriteLine("Error after slCreateEngine");
		return;
	}
	result = (*m_engineObj)->Realize(m_engineObj, SL_BOOLEAN_FALSE);
	if (result != SL_RESULT_SUCCESS) {
		LogWriter::WriteLine("Error after Realize m_engineObj");
		return;
	}
	result = (*m_engineObj)->GetInterface(m_engineObj, SL_IID_ENGINE,&m_engine);
	if (result != SL_RESULT_SUCCESS) {
		LogWriter::WriteLine("Error after GetInterface m_engine");
		return;
	}
	const SLInterfaceID pOutputMixIDs[] = {};
	const SLboolean pOutputMixRequired[] = {};
	/*Аналогично slCreateEngine()*/
	result = (*m_engine)->CreateOutputMix(m_engine, &m_outputMixObj, 0, pOutputMixIDs, pOutputMixRequired);
	if (result != SL_RESULT_SUCCESS) {
		LogWriter::WriteLine("Error after CreateOutputMix");
		return;
	}
	result = (*m_outputMixObj)->Realize(m_outputMixObj, SL_BOOLEAN_FALSE);
	if (result != SL_RESULT_SUCCESS) {
		LogWriter::WriteLine("Error after Realize m_outputMixObj");
		return;
	}
}

struct ResourseDescriptor {
	int32_t decriptor;
	off_t start;
	off_t length;
};
ResourseDescriptor loadResourceDescriptor(const char* path) {
	AAssetManager * assetManager = nullptr;
	AAsset* asset = AAssetManager_open(assetManager, path, AASSET_MODE_UNKNOWN);
	ResourseDescriptor resourceDescriptor;
	resourceDescriptor.decriptor = AAsset_openFileDescriptor(asset, &resourceDescriptor.start, &resourceDescriptor.length);
	AAsset_close(asset);
	return resourceDescriptor;
}

void CSoundPlayerOpenSLES::Play(const std::wstring& channel, const Path& file, float volume /*= 1.0f*/)
{
	ResourseDescriptor resourceDescriptor = loadResourceDescriptor("myMusic.mp3");
	SLDataLocator_AndroidFD locatorIn = {
		SL_DATALOCATOR_ANDROIDFD,
		resourceDescriptor.decriptor,
		resourceDescriptor.start,
		resourceDescriptor.length
	};

	SLDataFormat_MIME dataFormat = {
		SL_DATAFORMAT_MIME,
		NULL,
		SL_CONTAINERTYPE_UNSPECIFIED
	};

	SLDataSource audioSrc = { &locatorIn, &dataFormat };

	SLDataLocator_OutputMix dataLocatorOut = {
		SL_DATALOCATOR_OUTPUTMIX,
		m_outputMixObj
	};

	SLDataSink audioSnk = { &dataLocatorOut, NULL };
	const SLInterfaceID pIDs[2] = { SL_IID_PLAY, SL_IID_SEEK };
	const SLboolean pIDsRequired[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
	SLObjectItf playerObj;
	SLresult result = (*m_engine)->CreateAudioPlayer(m_engine, &playerObj, &audioSrc, &audioSnk, 2, pIDs, pIDsRequired);
	result = (*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE);
	SLPlayItf player;
	result = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &player);
	(*player)->SetPlayState(player, SL_PLAYSTATE_PLAYING);
}

void CSoundPlayerOpenSLES::PlaySoundPosition(const std::wstring& channel, const Path& file, const CVector3f& position, float volume /*= 1.0f*/)
{
	Play(channel, file, volume);
}

void CSoundPlayerOpenSLES::PlaySoundPlaylist(const std::wstring& name, const std::vector<Path>& files, float volume /*= 1.0f*/, bool shuffle /*= false*/, bool repeat /*= false*/)
{
}

void CSoundPlayerOpenSLES::SetListenerPosition(const CVector3f& position, const CVector3f& center)
{
}

void CSoundPlayerOpenSLES::PauseChannel(const std::wstring& name, bool pause)
{
}

void CSoundPlayerOpenSLES::StopChannel(const std::wstring& name)
{
}

void CSoundPlayerOpenSLES::Update()
{
}
