#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <tuple>
#include <map>
#include "../view/IShaderManager.h"
#include "VulkanHelpers.h"
#include <deque>

class CVulkanMemoryManager;
class CVulkanRenderer;

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
	void FreeMemoryDelayed(CVulkanMemory * memory);
	std::unique_ptr<CVulkanMemory> Allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags usageProperties);
	void FreeResources();
	void SetResourceFreeDelay(int delay) { m_resourceFreeDelay = delay; }
private:
	void FreeMemoryImpl(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size);
	VkDevice m_device;
	VkPhysicalDeviceMemoryProperties m_memory_properties;
	struct MemoryChunk
	{
		VkDeviceMemory memory;
		std::map<VkDeviceSize, VkDeviceSize> freeMemory;
		uint32_t memoryTypeBits;
		VkMemoryPropertyFlags usageFlags;
	};
	std::vector<MemoryChunk> m_chunks;
	VkDeviceSize m_chunkSize;
	std::deque<std::tuple<VkDeviceMemory, VkDeviceSize, VkDeviceSize, int>> m_delayedFreeMemory;
	int m_resourceFreeDelay = 1000;
};

class CVulkanVertexAttribCache : public IVertexAttribCache
{
public:
	CVulkanVertexAttribCache(VkDeviceSize size, VkBufferUsageFlags flags, CVulkanRenderer & renderer, VkMemoryPropertyFlags properties, const void * data = nullptr);
	CVulkanVertexAttribCache(const CVulkanVertexAttribCache & other) = delete;
	CVulkanVertexAttribCache(CVulkanVertexAttribCache && other) = default;
	CVulkanVertexAttribCache& operator=(CVulkanVertexAttribCache const& other) = delete;
	~CVulkanVertexAttribCache();
	void Upload(const void* data, VkDeviceSize size);
	void UploadStaged(const void* data, VkDeviceSize size, VkCommandBuffer commandBuffer);
	operator VkBuffer() const { return m_buffer; }
private:
	CVulkanRenderer * m_renderer;
	VkBuffer m_buffer = VK_NULL_HANDLE;
	std::unique_ptr<CVulkanMemory> m_memory;
	VkBufferUsageFlags m_flags;
};

class CVulkanSmartBuffer
{
public:
	CVulkanSmartBuffer(size_t chunkSize, VkBufferUsageFlags flags, CVulkanRenderer & renderer, VkFlags properties);
	CVulkanSmartBuffer(const CVulkanSmartBuffer & other) = delete;
	CVulkanSmartBuffer(CVulkanSmartBuffer && other) = default;
	std::tuple<VkBuffer, size_t, void*> Allocate(size_t size);
	void Commit(bool clear = true);
private:
	struct Buffer
	{
		Buffer(size_t size, VkBufferUsageFlags flags, CVulkanRenderer & renderer, VkFlags properties)
			: buffer(std::make_unique<CVulkanVertexAttribCache>(size, flags, renderer, properties))
		{}
		Buffer(const Buffer & other) = delete;
		Buffer(Buffer && other) = default;
		Buffer& operator=(const Buffer & other) = delete;
		~Buffer() = default;
		std::unique_ptr<CVulkanVertexAttribCache> buffer;
		std::vector<char> cache;
	};
	std::vector<Buffer> m_chunks;
	size_t m_chunkSize;
	VkBufferUsageFlags m_usageflags;
	VkFlags m_properties;
	CVulkanRenderer & m_renderer;
};
