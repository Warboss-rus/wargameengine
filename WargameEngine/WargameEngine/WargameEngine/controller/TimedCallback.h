#include <string>
#include <map>

struct sTimedCallback
{
	std::string function;
	unsigned int time;
};

class CTimedCallback
{
public:
	static CTimedCallback * GetInstance();
	static void FreeInstance();
	unsigned int AddCallback(std::string const& callback, unsigned int time, bool repeat);
	void DeleteCallback(unsigned int index);
	const sTimedCallback * GetCallback(unsigned int index);
private:
	static CTimedCallback * m_self;
	std::map<unsigned int, sTimedCallback> m_callbacks;
};