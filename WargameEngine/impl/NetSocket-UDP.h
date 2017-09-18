class CNetSocket
{
public:
	CNetSocket(unsigned short port = 0);
	CNetSocket(const char * ip, unsigned short port = 0);
	bool ChangeAddress(const char* ip, unsigned short port = 0);
	bool ChangeAddress(unsigned short port = 0);
	bool SendData(const char * data, int len) const;
	bool RecieveData(char* data, int maxLength);
	const char* GetIP() const;
	unsigned short GetPort() const;
	~CNetSocket();
private:
	void InitSocket();
	unsigned int m_socket;
	void * m_sockAddr;
};