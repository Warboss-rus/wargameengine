#include "VulkanShaderManager.h"
#include "../Utils.h"
#include "../LogWriter.h"

VkShaderModule CompileShader(std::wstring const& filename, VkDevice device)
{
	if (filename.empty()) return VK_NULL_HANDLE;
	const std::vector<char> code = ReadFile(filename);
	if (code.empty()) return VK_NULL_HANDLE;
	VkShaderModuleCreateInfo shaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };
	VkShaderModule shaderModule;
	VkResult result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule);
	if (result)
	{
		LogWriter::WriteLine(L"Cannot create shader module for " + filename);
	}
	return shaderModule;
}

std::unique_ptr<IShaderProgram> CVulkanShaderManager::NewProgram(std::wstring const& vertex /*= L""*/, std::wstring const& fragment /*= L""*/, std::wstring const& geometry /*= L""*/)
{
	VkShaderModule vertexShader = CompileShader(vertex, m_device);
	VkShaderModule fragmentShader = CompileShader(fragment, m_device);
	VkShaderModule geometryShader = CompileShader(geometry, m_device);
	auto program = std::make_unique<CVulkanShaderProgram>(m_device);
	program->AddShaderModule(vertexShader, VK_SHADER_STAGE_VERTEX_BIT);
	program->AddShaderModule(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT);
	program->AddShaderModule(geometryShader, VK_SHADER_STAGE_GEOMETRY_BIT);
	return std::move(program);
}

void CVulkanShaderManager::PushProgram(IShaderProgram const& program) const
{
}

void CVulkanShaderManager::PopProgram() const
{
}

void CVulkanShaderManager::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const
{
}

void CVulkanShaderManager::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const
{
}

void CVulkanShaderManager::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const
{
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance /*= false*/) const
{
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance /*= false*/) const
{
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance /*= false*/) const
{
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, bool perInstance /*= false*/) const
{
}

void CVulkanShaderManager::DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const
{
}

void CVulkanShaderManager::DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const
{
}

void CVulkanShaderManager::DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const
{
}

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(int elementSize, size_t count, const float* value) const
{
	return nullptr;
}

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(int elementSize, size_t count, const int* value) const
{
	return nullptr;
}

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(int elementSize, size_t count, const unsigned int* value) const
{
	return nullptr;
}

void CVulkanShaderManager::SetDevice(VkDevice device)
{
	m_device = device;
}

CVulkanShaderProgram::CVulkanShaderProgram(VkDevice device)
	:m_device(device)
{
}

void CVulkanShaderProgram::AddShaderModule(VkShaderModule module, VkShaderStageFlagBits flag)
{
	if (module)
	{
		m_modules.push_back(module);
		m_shaderStageCreateInfos.push_back({ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, flag, module, "main", nullptr });
	}
}

const std::vector<VkPipelineShaderStageCreateInfo>& CVulkanShaderProgram::GetShaderInfo() const
{
	return m_shaderStageCreateInfos;
}

CVulkanShaderProgram::~CVulkanShaderProgram()
{
	for (VkShaderModule module : m_modules)
	{
		vkDestroyShaderModule(m_device, module, nullptr);
	}
}

CVulkanVertexBuffer::CVulkanVertexBuffer(size_t size, VkDevice device, VkPhysicalDevice physicalDevice, const void * data)
	:m_device(device), m_size(size)
{
	m_buffer.SetDevice(device);
	m_memory.SetDevice(device);
	VkBufferCreateInfo buffer_create_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr };
	VkResult result = vkCreateBuffer(m_device, &buffer_create_info, nullptr, &m_buffer);
	if (result)
	{
		LogWriter::WriteLine("Cannot create buffer");
	}

	VkMemoryRequirements buffer_memory_requirements; 
	vkGetBufferMemoryRequirements(m_device, m_buffer, &buffer_memory_requirements);
	VkPhysicalDeviceMemoryProperties memory_properties; 
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory_properties);
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
	{
		if ((buffer_memory_requirements.memoryTypeBits & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
		{
			VkMemoryAllocateInfo memory_allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, buffer_memory_requirements.size, i };
			if (vkAllocateMemory(m_device, &memory_allocate_info, nullptr, &m_memory) == VK_SUCCESS)
			{
				result = vkBindBufferMemory(m_device, m_buffer, m_memory, 0);
				if (result)
				{
					LogWriter::WriteLine("Cannot bind memory");
				}
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

void CVulkanVertexBuffer::Upload(const void* data, size_t size)
{
	void *vertex_buffer_memory_pointer;
	VkResult result = vkMapMemory(m_device, m_memory, 0, size, 0, &vertex_buffer_memory_pointer);
	if (result)
	{
		LogWriter::WriteLine("Cannot map memory");
	}
	memcpy(vertex_buffer_memory_pointer, data, size);
	VkMappedMemoryRange flush_range = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE, nullptr, m_memory, 0, VK_WHOLE_SIZE };
	result = vkFlushMappedMemoryRanges(m_device, 1, &flush_range);
	if (result)
	{
		LogWriter::WriteLine("Cannot flush memory");
	}
	vkUnmapMemory(m_device, m_memory);
}

size_t CVulkanVertexBuffer::GetSize() const
{
	return m_size;
}
