#ifdef WIN32
#define PLUGIN_API __declspec(dllexport)
#endif

class IClass
{
public:
	virtual void SetParam(int x) = 0;
	virtual int GetParam() = 0;
};

class CClass : public IClass
{
public:
	void SetParam(int x) { m_param = x; }
	int GetParam() { return m_param; }
private:
	int m_param;
};

extern "C" {
	PLUGIN_API const char* GetType()
	{
		return "image";
	}

	PLUGIN_API IClass * GetClass()
	{
		return new CClass();
	}
}
