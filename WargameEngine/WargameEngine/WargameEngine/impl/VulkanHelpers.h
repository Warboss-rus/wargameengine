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

template <class T, VKAPI_ATTR void (VKAPI_CALL*Deleter)(T, const VkAllocationCallbacks *)>
class CInstanceWrapper
{
public:
	~CInstanceWrapper() { Destroy(); }
	operator T () { return m_data; }
	operator const T() const { return m_data; }
	T* operator & () { return &m_data; }
	void Destroy()
	{
		if (m_data)
		{
			Deleter(m_data, nullptr);
			m_data = VK_NULL_HANDLE;
		}
	}
private:
	T m_data = VK_NULL_HANDLE;
};

class CDestructor
{
public:
	~CDestructor()
	{
		if (m_func) m_func();
	}
	void SetDestructorFunction(std::function<void()> const& func) { m_func = func; }
private:
	std::function<void()> m_func;
};

#define CHECK_VK_RESULT(result, message) if(result) throw std::runtime_error(message)
#define LOG_VK_RESULT(result, message) if(result) LogWriter::WriteLine(message)