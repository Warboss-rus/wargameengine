#include <map>
#include <vector>
#include <memory>

class CSoundPlayer
{
public:
	static std::weak_ptr<CSoundPlayer> GetInstance();
	static void FreeInstance();
	void PlaySound(std::string const& file);
	~CSoundPlayer();
private:
	CSoundPlayer();
	static std::shared_ptr<CSoundPlayer> m_instance;
	void ReadWav(std::string const& file);
	void * m_device;
	void * m_context;
	std::map<std::string, unsigned int> m_buffers;
	std::vector<unsigned int> m_sources;
};