#include "VulkanShaderManager.h"
#include "../Utils.h"
#include "../LogWriter.h"
#include <spirv_glsl.hpp>
#include <algorithm>

ShaderReflection ReflectShader(const std::vector<char> & code)
{
	std::vector<uint32_t> codeUint(code.size() * sizeof(char) / (sizeof(uint32_t)));
	memcpy(codeUint.data(), code.data(), code.size() * sizeof(char));
	spirv_cross::CompilerGLSL glsl(std::move(codeUint));
	spirv_cross::ShaderResources resources = glsl.get_shader_resources();
	ShaderReflection reflection;
	reflection.bufferSize = 0;
	for (auto& uniformBuffer : resources.uniform_buffers)
	{
		std::vector<spirv_cross::BufferRange> ranges = glsl.get_active_buffer_ranges(uniformBuffer.id);
		for (auto& range : ranges)
		{
			std::string name = glsl.get_member_name(uniformBuffer.base_type_id, range.index);
			reflection.uniforms.push_back({name, uniformBuffer.name, range.offset, range.range});
			reflection.bufferSize += range.range;
		}
	}
	for (auto& input : resources.stage_inputs)
	{
		reflection.attributes.push_back(input.name);
	}
	return reflection;
}

VkShaderModule CompileShader(std::wstring const& filename, VkDevice device, ShaderReflection * reflection = nullptr)
{
	if (filename.empty()) return VK_NULL_HANDLE;
	const std::vector<char> code = ReadFile(filename);
	if (code.empty()) return VK_NULL_HANDLE;
	if (reflection)
	{
		*reflection = ReflectShader(code);
	}
	VkShaderModuleCreateInfo shaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0, code.size(), reinterpret_cast<const uint32_t*>(code.data()) };
	VkShaderModule shaderModule;
	VkResult result = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule);
	LOG_VK_RESULT(result, L"Cannot create shader module for " + filename);
	return shaderModule;
}

std::unique_ptr<IShaderProgram> CVulkanShaderManager::NewProgram(std::wstring const& vertex /*= L""*/, std::wstring const& fragment /*= L""*/, std::wstring const& geometry /*= L""*/)
{
	ShaderReflection vertexReflection, fragmentReflection, geometryReflection;
	VkShaderModule vertexShader = CompileShader(vertex, m_device, &vertexReflection);
	VkShaderModule fragmentShader = CompileShader(fragment, m_device, &fragmentReflection);
	VkShaderModule geometryShader = CompileShader(geometry, m_device, &geometryReflection);
	auto program = std::make_unique<CVulkanShaderProgram>(m_device);
	program->AddShaderModule(vertexShader, VK_SHADER_STAGE_VERTEX_BIT, vertexReflection, m_physicalDevice);
	program->AddShaderModule(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, fragmentReflection, m_physicalDevice);
	program->AddShaderModule(geometryShader, VK_SHADER_STAGE_GEOMETRY_BIT, geometryReflection, m_physicalDevice);

	VkDescriptorBufferInfo bufferInfo = {};

	return std::move(program);
}

void CVulkanShaderManager::PushProgram(IShaderProgram const& program) const
{
	auto& vulkanProgram = reinterpret_cast<const CVulkanShaderProgram&>(program);
	m_programsStack.push_back(&vulkanProgram);
	if (m_onProgramChange) m_onProgramChange(*m_programsStack.back());
}

void CVulkanShaderManager::PopProgram() const
{
	m_programsStack.pop_back();
	if (m_onProgramChange) m_onProgramChange(*m_programsStack.back());
}

void CVulkanShaderManager::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const
{
	m_programsStack.back()->SetUniformValue(uniform, value, elementSize * count * sizeof(float));
}

void CVulkanShaderManager::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const
{
	m_programsStack.back()->SetUniformValue(uniform, value, elementSize * count * sizeof(int));
}

void CVulkanShaderManager::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const
{
	m_programsStack.back()->SetUniformValue(uniform, value, elementSize * count * sizeof(unsigned));
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

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, bool perInstance /*= false*/, size_t offset /*= 0*/) const
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
	return std::make_unique<CStagedVulkanVertexAttribCache>(elementSize * count * sizeof(float), CVulkanVertexAttribCache::BufferType::VERTEX, m_device, m_physicalDevice, value);
}

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(int elementSize, size_t count, const int* value) const
{
	return std::make_unique<CStagedVulkanVertexAttribCache>(elementSize * count * sizeof(int), CVulkanVertexAttribCache::BufferType::VERTEX, m_device, m_physicalDevice, value);
}

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(int elementSize, size_t count, const unsigned int* value) const
{
	return std::make_unique<CStagedVulkanVertexAttribCache>(elementSize * count * sizeof(unsigned), CVulkanVertexAttribCache::BufferType::VERTEX, m_device, m_physicalDevice, value);
}

void CVulkanShaderManager::SetDevice(VkDevice device, VkPhysicalDevice physicalDevice)
{
	m_device = device;
	m_physicalDevice = physicalDevice;
}

void CVulkanShaderManager::DoOnProgramChange(std::function<void(const CVulkanShaderProgram&)> const& handler)
{
	m_onProgramChange = handler;
}

CVulkanShaderProgram::CVulkanShaderProgram(VkDevice device)
	:m_device(device)
{
}

void CVulkanShaderProgram::AddShaderModule(VkShaderModule module, VkShaderStageFlagBits flag, ShaderReflection const& reflection, VkPhysicalDevice physicalDevice)
{
	if (module)
	{
		m_modules.push_back(module);
		m_shaderStageCreateInfos.push_back({ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, flag, module, "main", nullptr });
		m_uniformBuffers.emplace_back();
		m_uniformBuffers.back().cache.resize(reflection.bufferSize);
		m_uniformBuffers.back().buffer = std::make_unique<CVulkanVertexAttribCache>(reflection.bufferSize, CVulkanVertexAttribCache::BufferType::UNIFORM, m_device, physicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		m_uniformBuffers.back().reflection = reflection;
	}
}

const std::vector<VkPipelineShaderStageCreateInfo>& CVulkanShaderProgram::GetShaderInfo() const
{
	return m_shaderStageCreateInfos;
}

void CVulkanShaderProgram::SetUniformValue(std::string const& name, const void * data, size_t size) const
{
	for (auto& buffer : m_uniformBuffers)
	{
		auto& uniforms = buffer.reflection.uniforms;
		auto it = std::find_if(uniforms.begin(), uniforms.end(), [&name](ShaderReflection::UniformDescription const& description) { return description.name == name; });
		if (it != uniforms.end())
		{
			memcpy(buffer.cache.data() + it->offset, data, size);
			buffer.buffer->Upload(buffer.cache.data(), buffer.cache.size() * sizeof(char));
		}
	}
}

CVulkanShaderProgram::~CVulkanShaderProgram()
{
	for (VkShaderModule module : m_modules)
	{
		vkDestroyShaderModule(m_device, module, nullptr);
	}
}

CVulkanVertexAttribCache::CVulkanVertexAttribCache(size_t size, BufferType type, VkDevice device, VkPhysicalDevice physicalDevice, VkFlags properties, const void * data)
	:m_device(device), m_size(size)
{
	m_buffer.SetDevice(device);
	m_memory.SetDevice(device);
	if (size == 0) return;
	const std::map<BufferType, VkFlags> typeMap = {
		{ BufferType::VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT },
		{ BufferType::INDEX, VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
		{ BufferType::UNIFORM, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
	};
	VkBufferCreateInfo buffer_create_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0, size, typeMap.at(type) | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr };
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

CStagedVulkanVertexAttribCache::CStagedVulkanVertexAttribCache(size_t size, CVulkanVertexAttribCache::BufferType type, VkDevice device, VkPhysicalDevice physicalDevice, const void * data /*= nullptr*/)
	: m_deviceBuffer(size, type, device, physicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	, m_stageBuffer(size, type, device, physicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
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
