#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <tuple>
#include <map>
#include "../view/IShaderManager.h"
#include "VulkanHelpers.h"

class CVulkanMemoryManager;

class CVulkanMemory
{
public:
	CVulkanMemory(CVulkanMemoryManager & manager, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size);
	CVulkanMemory(CVulkanMemory const& other) = delete;
	CVulkanMemory& operator=(CVulkanMemory const& other) = delete;
	CVulkanMemory& operator=(CVulkanMemory && other) = default;
	~CVulkanMemory();

	operator VkDeviceMemory() const { return m_memory; }
	VkDeviceSize GetOffset() const { return m_offset; }
	VkDeviceSize GetSize() const { return m_size; }
private:
	CVulkanMemoryManager & m_manager;
	VkDeviceMemory m_memory;
	VkDeviceSize m_offset;
	VkDeviceSize m_size;
};

class CVulkanMemoryManager
{
public:
	CVulkanMemoryManager(VkDeviceSize chunkSize, VkDevice device, VkPhysicalDevice physicalDevice);
	~CVulkanMemoryManager();
	void FreeMemory(CVulkanMemory * memory);
	std::unique_ptr<CVulkanMemory> Allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags usageProperties);
private:
	VkDevice m_device;
	VkPhysicalDeviceMemoryProperties m_memory_properties;
	struct MemoryChunk
	{
		VkDeviceMemory memory;
		std::map<VkDeviceSize, VkDeviceSize> freeMemory;
		uint32_t memoryTypeBits;
		VkMemoryPropertyFlags usageFlags;
	};
	std::vector<MemoryChunk> m_ñhunks;
	VkDeviceSize m_chunkSize;
};

class CVulkanVertexAttribCache : public IVertexAttribCache
{
public:
	CVulkanVertexAttribCache(size_t size, VkBufferUsageFlags flags, VkDevice device, VkPhysicalDevice physicalDevice, VkFlags properties, const void * data = nullptr);
	void Upload(const void* data, size_t size);
	size_t GetSize() const;
	operator VkBuffer() const { return m_buffer; }
private:
	VkDevice m_device;
	CHandleWrapper<VkBuffer, vkDestroyBuffer> m_buffer;
	CHandleWrapper<VkDeviceMemory, vkFreeMemory> m_memory;
	size_t m_size;
};

class CStagedVulkanVertexAttribCache : public IVertexAttribCache
{
public:
	CStagedVulkanVertexAttribCache(size_t size, VkBufferUsageFlags flags, VkDevice device, VkPhysicalDevice physicalDevice);
	void Upload(const void* data, size_t size, VkCommandBuffer commandBuffer);
	size_t GetSize() const { return m_stageBuffer.GetSize(); }
	operator VkBuffer() const { return m_deviceBuffer; }
private:
	CVulkanVertexAttribCache m_deviceBuffer;
	CVulkanVertexAttribCache m_stageBuffer;
};

class CVulkanSmartBuffer
{
public:
	CVulkanSmartBuffer(size_t chunkSize, VkBufferUsageFlags flags, VkDevice device, VkPhysicalDevice physicalDevice, VkFlags properties);
	CVulkanSmartBuffer(const CVulkanSmartBuffer & other) = delete;
	CVulkanSmartBuffer(CVulkanSmartBuffer && other) = default;
	std::tuple<VkBuffer, size_t, void*> Allocate(size_t size);
	void Commit(bool clear = true);
private:
	struct Buffer
	{
		Buffer(size_t size, VkBufferUsageFlags flags, VkDevice device, VkPhysicalDevice physicalDevice, VkFlags properties)
			: buffer(size, flags, device, physicalDevice, properties)
		{}
		Buffer(const Buffer & other) = delete;
		Buffer(Buffer && other) = default;
		~Buffer() = default;
		CVulkanVertexAttribCache buffer;
		std::vector<char> cache;
	};
	std::vector<Buffer> m_chunks;
	size_t m_chunkSize;
	VkBufferUsageFlags m_usageflags;
	VkFlags m_properties;
	VkDevice m_device;
	VkPhysicalDevice m_physicalDevice;
};
