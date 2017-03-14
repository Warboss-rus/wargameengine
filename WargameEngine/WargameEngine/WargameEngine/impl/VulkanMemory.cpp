#include "VulkanMemory.h"
#include "../LogWriter.h"
#include <algorithm>

CVulkanVertexAttribCache::CVulkanVertexAttribCache(size_t size, VkBufferUsageFlags flags, VkDevice device, VkPhysicalDevice physicalDevice, VkFlags properties, const void * data)
	:m_device(device), m_size(size)
{
	m_buffer.SetDevice(device);
	m_memory.SetDevice(device);
	if (size == 0) return;
	VkBufferCreateInfo buffer_create_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0, size, flags, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr };
	VkResult result = vkCreateBuffer(m_device, &buffer_create_info, nullptr, &m_buffer);
	LOG_VK_RESULT(result, "Cannot create buffer");

	VkMemoryRequirements buffer_memory_requirements;
	vkGetBufferMemoryRequirements(m_device, m_buffer, &buffer_memory_requirements);
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory_properties);
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
	{
		if ((buffer_memory_requirements.memoryTypeBits & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties))
		{
			VkMemoryAllocateInfo memory_allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, buffer_memory_requirements.size, i };
			if (vkAllocateMemory(m_device, &memory_allocate_info, nullptr, &m_memory) == VK_SUCCESS)
			{
				result = vkBindBufferMemory(m_device, m_buffer, m_memory, 0);
				LOG_VK_RESULT(result, "Cannot bind memory");
				if (data)
				{
					Upload(data, size);
				}
				return;
			}
		}
	}
	LogWriter::WriteLine("Cannot allocate device memory");
}

void CVulkanVertexAttribCache::Upload(const void* data, size_t size)
{
	void *vertex_buffer_memory_pointer;
	VkResult result = vkMapMemory(m_device, m_memory, 0, size, 0, &vertex_buffer_memory_pointer);
	LOG_VK_RESULT(result, "Cannot map memory");
	memcpy(vertex_buffer_memory_pointer, data, size);
	VkMappedMemoryRange flush_range = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE, nullptr, m_memory, 0, VK_WHOLE_SIZE };
	result = vkFlushMappedMemoryRanges(m_device, 1, &flush_range);
	LOG_VK_RESULT(result, "Cannot flush memory");
	vkUnmapMemory(m_device, m_memory);
}

size_t CVulkanVertexAttribCache::GetSize() const
{
	return m_size;
}

CStagedVulkanVertexAttribCache::CStagedVulkanVertexAttribCache(size_t size, VkBufferUsageFlags flags, VkDevice device, VkPhysicalDevice physicalDevice)
	: m_deviceBuffer(size, flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT, device, physicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	, m_stageBuffer(size, flags | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, device, physicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
{
}

void CStagedVulkanVertexAttribCache::Upload(const void* data, size_t size, VkCommandBuffer commandBuffer)
{
	m_stageBuffer.Upload(data, size);
	VkBufferCopy buffer_copy_info = { 0, 0, size };
	vkCmdCopyBuffer(commandBuffer, m_stageBuffer, m_deviceBuffer, 1, &buffer_copy_info);

	VkBufferMemoryBarrier buffer_memory_barrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, nullptr, VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED, m_deviceBuffer, 0, VK_WHOLE_SIZE };
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &buffer_memory_barrier, 0, nullptr);
}

CVulkanSmartBuffer::CVulkanSmartBuffer(size_t chunkSize, VkBufferUsageFlags flags, VkDevice device, VkPhysicalDevice physicalDevice, VkFlags properties)
	:m_chunkSize(chunkSize), m_usageflags(flags), m_properties(properties), m_device(device), m_physicalDevice(physicalDevice)
{
}

std::tuple<VkBuffer, size_t, void*> CVulkanSmartBuffer::Allocate(size_t size)
{
	for (auto& chunk : m_chunks)
	{
		size_t oldSize = chunk.cache.size();
		if (oldSize + size < m_chunkSize)
		{
			chunk.cache.resize(oldSize + size);
			return std::make_tuple(static_cast<VkBuffer>(chunk.buffer), oldSize, chunk.cache.data() + oldSize);
		}
	}
	//allocate new chunk
	m_chunks.emplace_back(std::max(m_chunkSize, size), m_usageflags, m_device, m_physicalDevice, m_properties);
	auto& newChunk = m_chunks.back();
	newChunk.cache.reserve(std::max(m_chunkSize, size));
	newChunk.cache.resize(size);
	return std::make_tuple(static_cast<VkBuffer>(newChunk.buffer), 0, newChunk.cache.data());
}

void CVulkanSmartBuffer::Commit(bool clear)
{
	for (auto& chunk : m_chunks)
	{
		if (!chunk.cache.empty())
		{
			chunk.buffer.Upload(chunk.cache.data(), chunk.cache.size());
			if (clear) chunk.cache.clear();
		}
	}
}
