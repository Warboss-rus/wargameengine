#include "VulkanMemory.h"
#include "../LogWriter.h"
#include <algorithm>
#include "VulkanRenderer.h"

CVulkanVertexAttribCache::CVulkanVertexAttribCache(VkDeviceSize size, VkBufferUsageFlags flags, CVulkanRenderer & renderer, VkMemoryPropertyFlags properties, const void * data)
	: m_renderer(&renderer), m_flags(flags)
{
	if (size == 0) return;
	VkDevice device = renderer.GetDevice();
	VkBufferCreateInfo buffer_create_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0, size, flags, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr };
	VkResult result = vkCreateBuffer(device, &buffer_create_info, nullptr, &m_buffer);
	LOG_VK_RESULT(result, "Cannot create buffer");

	VkMemoryRequirements buffer_memory_requirements;
	vkGetBufferMemoryRequirements(device, m_buffer, &buffer_memory_requirements);
	m_memory = renderer.GetMemoryManager().Allocate(buffer_memory_requirements, properties);
	if (!m_memory)
	{
		wargameEngine::LogWriter::WriteLine("Cannot allocate device memory");
		return;
	}
	result = vkBindBufferMemory(device, m_buffer, *m_memory, m_memory->GetOffset());
	LOG_VK_RESULT(result, "Cannot bind memory");
	if (data)
	{
		Upload(data, size);
	}
}

CVulkanVertexAttribCache::~CVulkanVertexAttribCache()
{
	m_renderer->DestroyBuffer(m_buffer);
}

void CVulkanVertexAttribCache::Upload(const void* data, VkDeviceSize size)
{
	VkDevice device = m_renderer->GetDevice();
	void *vertex_buffer_memory_pointer;
	VkResult result = vkMapMemory(device, *m_memory, m_memory->GetOffset(), size, 0, &vertex_buffer_memory_pointer);
	LOG_VK_RESULT(result, "Cannot map memory");
	memcpy(vertex_buffer_memory_pointer, data, static_cast<size_t>(size));
	VkMappedMemoryRange flush_range = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE, nullptr, *m_memory, m_memory->GetOffset(), size };
	result = vkFlushMappedMemoryRanges(device, 1, &flush_range);
	LOG_VK_RESULT(result, "Cannot flush memory");
	vkUnmapMemory(device, *m_memory);
}

void CVulkanVertexAttribCache::UploadStaged(const void* data, VkDeviceSize size, VkCommandBuffer commandBuffer)
{
	CVulkanVertexAttribCache stagingBuffer(size, (m_flags & ~VK_BUFFER_USAGE_TRANSFER_DST_BIT) | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, *m_renderer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	stagingBuffer.Upload(data, size);

	VkBufferCopy buffer_copy_info = { 0, 0, size };
	vkCmdCopyBuffer(commandBuffer, stagingBuffer, m_buffer, 1, &buffer_copy_info);

	VkBufferMemoryBarrier buffer_memory_barrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, nullptr, VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED, m_buffer, 0, VK_WHOLE_SIZE };
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &buffer_memory_barrier, 0, nullptr);
}

CVulkanSmartBuffer::CVulkanSmartBuffer(size_t chunkSize, VkBufferUsageFlags flags, CVulkanRenderer & renderer, VkFlags properties)
	:m_chunkSize(chunkSize), m_usageflags(flags), m_properties(properties), m_renderer(renderer)
{
}

std::tuple<VkBuffer, VkDeviceSize, void*> CVulkanSmartBuffer::Allocate(size_t size)
{
	for (auto& chunk : m_chunks)
	{
		VkDeviceSize oldSize = chunk.cache.size();
		if (oldSize + size <= chunk.size)
		{
			chunk.cache.resize(static_cast<size_t>(oldSize) + size);
			return std::make_tuple(static_cast<VkBuffer>(*chunk.buffer), oldSize, chunk.cache.data() + oldSize);
		}
	}
	//allocate new chunk
	m_chunks.emplace_back(std::max(m_chunkSize, size), m_usageflags, m_renderer, m_properties);
	auto& newChunk = m_chunks.back();
	newChunk.cache.reserve(std::max(m_chunkSize, size));
	newChunk.cache.resize(size);
	return std::tuple<VkBuffer, VkDeviceSize, void*>(*newChunk.buffer, 0, newChunk.cache.data());
}

void CVulkanSmartBuffer::Commit()
{
	for (auto& chunk : m_chunks)
	{
		if (!chunk.cache.empty())
		{
			chunk.buffer->Upload(chunk.cache.data(), chunk.cache.size());
			chunk.cache.clear();
		}
	}
}

std::vector<VkBuffer> CVulkanSmartBuffer::GetAllBuffers() const
{
	std::vector<VkBuffer> result;
	std::transform(m_chunks.begin(), m_chunks.end(), std::back_inserter(result), [](Buffer const& buffer) {
		return (VkBuffer)*buffer.buffer;
	});
	return result;
}

CVulkanMemoryManager::CVulkanMemoryManager(VkDeviceSize chunkSize, VkDevice device, VkPhysicalDevice physicalDevice)
	: m_device(device), m_chunkSize(chunkSize)
{
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &m_memory_properties);
}

CVulkanMemoryManager::~CVulkanMemoryManager()
{
	for (auto& chunk : m_chunks)
	{
		vkFreeMemory(m_device, chunk.memory, nullptr);
	}
}

void CVulkanMemoryManager::FreeMemory(CVulkanMemory * memory)
{
	FreeMemoryImpl(*memory, memory->GetOffset(), memory->GetSize());
}

void CVulkanMemoryManager::FreeMemoryDelayed(CVulkanMemory * memory)
{
	m_delayedFreeMemory.push_back(std::make_tuple((VkDeviceMemory)*memory, memory->GetOffset(), memory->GetSize(), 1000));
}

VkDeviceSize AlignDown(VkDeviceSize value, VkDeviceSize alignment)
{
	return value / alignment * alignment;
}

std::unique_ptr<CVulkanMemory> CVulkanMemoryManager::Allocate(VkMemoryRequirements requirements, VkMemoryPropertyFlags usageProperties)
{
	for (auto& chunk : m_chunks)
	{
		if ((chunk.memoryTypeBits & requirements.memoryTypeBits) && (chunk.usageFlags & usageProperties))
		{
			for (auto& block : chunk.freeMemory)
			{
				if (block.second >= requirements.size)
				{
					VkDeviceSize begin = block.first + block.second - requirements.size;
					VkDeviceSize beginAligned = AlignDown(begin, requirements.alignment);
					VkDeviceSize alignedSize = begin - beginAligned + requirements.size;
					auto memory = std::make_unique<CVulkanMemory>(*this, chunk.memory, beginAligned, alignedSize);
					block.second -= alignedSize;
					return memory;
				}
			}
		}
	}
	//allocate a new chunk
	for (uint32_t i = 0; i < m_memory_properties.memoryTypeCount; ++i)
	{
		uint32_t memoryTypeBits = 1u << i;
		if ((requirements.memoryTypeBits & memoryTypeBits) && (m_memory_properties.memoryTypes[i].propertyFlags & usageProperties))
		{
			VkMemoryAllocateInfo memory_allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, std::max(requirements.size, m_chunkSize), i };
			VkDeviceMemory memory;
			if (vkAllocateMemory(m_device, &memory_allocate_info, nullptr, &memory) == VK_SUCCESS)
			{
				if (requirements.size < m_chunkSize)
				{
					m_chunks.push_back({ memory, {{ requirements.size, m_chunkSize - requirements.size }}, memoryTypeBits, m_memory_properties.memoryTypes[i].propertyFlags });
				}
				return std::make_unique<CVulkanMemory>(*this, memory, 0, requirements.size);
			}
		}
	}
	return nullptr;
}

void CVulkanMemoryManager::FreeResources()
{
	for (auto& mem : m_delayedFreeMemory)
	{
		--std::get<3>(mem);
	}
	while(!m_delayedFreeMemory.empty() && (std::get<3>(m_delayedFreeMemory.front()) == 0))
	{
		auto& tuple = m_delayedFreeMemory.front();
		FreeMemoryImpl(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
		m_delayedFreeMemory.pop_front();
	}
}

void CVulkanMemoryManager::FreeMemoryImpl(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size)
{
	//find a chunk that contains this memory block
	auto it = std::find_if(m_chunks.begin(), m_chunks.end(), [memory](MemoryChunk const& chunk) {
		return chunk.memory == memory;
	});
	if (it == m_chunks.end()) return;
	//Add memory block to the list of free blocks
	auto insertedIt = it->freeMemory.insert({ offset, size }).first;
	//merge blocks if necessary
	auto mergeIt = it->freeMemory.find(offset + size);
	if (mergeIt != it->freeMemory.end())
	{
		insertedIt->second += mergeIt->second;
		it->freeMemory.erase(mergeIt);
	}
	mergeIt = std::find_if(it->freeMemory.begin(), it->freeMemory.end(), [offset](std::pair<VkDeviceSize, VkDeviceSize> const& pair) {
		return (pair.first + pair.second) == offset;
	});
	if (mergeIt != it->freeMemory.end())
	{
		mergeIt->second += size;
		it->freeMemory.erase(insertedIt);
	}
}

CVulkanMemory::CVulkanMemory(CVulkanMemoryManager & manager, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size)
	:m_manager(manager), m_memory(memory), m_offset(offset), m_size(size)
{
}

CVulkanMemory::~CVulkanMemory()
{
	m_manager.FreeMemoryDelayed(this);
}
