#include <map>
#include <vector>
#include <memory>
#include <future>
#include "view/Vector3.h"

class CSoundPlayer
{
public:
	static std::weak_ptr<CSoundPlayer> GetInstance();
	static void FreeInstance();
	void Play(std::string const& file, float volume = 1.0f);
	void PlaySoundPosition(std::string const& file, CVector3d const& position, float volume = 1.0f);
	void PlaySoundPlaylist(std::vector<std::string> const& files, float volume = 1.0f, bool shuffle = true, bool repeat = false);
	void SetListenerPosition(CVector3d const& position, CVector3d const& center);
	void Update();
	~CSoundPlayer();
private:
	CSoundPlayer() {}
	void Init();
	static std::shared_ptr<CSoundPlayer> m_instance;
	void ReadWav(std::string const& file);
	void * m_device;
	void * m_context;
	std::map<std::string, unsigned int> m_buffers;
	std::vector<unsigned int> m_sources;
	std::future<void> m_thread;
	bool m_stop;
};