#pragma once

template <class T, VKAPI_ATTR void (VKAPI_CALL*Deleter)(VkDevice, T, const VkAllocationCallbacks *)>
class CHandleWrapper
{
public:
	CHandleWrapper(VkDevice device = VK_NULL_HANDLE)
		:m_device(device)
	{}
	~CHandleWrapper() { Destroy(); }
	operator T () { return m_data; }
	operator const T() const { return m_data; }
	T* operator & () { return &m_data; }
	void SetDevice(VkDevice device)
	{
		m_device = device;
	}
	void Destroy()
	{
		if (m_device && m_data)
		{
			Deleter(m_device, m_data, nullptr);
			m_device = VK_NULL_HANDLE;
			m_data = VK_NULL_HANDLE;
		}
	}
private:
	T m_data = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;
};

#define CHECK_VK_RESULT(result, message) if(result) throw std::runtime_error(message)
#define LOG_VK_RESULT(result, message) if(result) LogWriter::WriteLine(message)