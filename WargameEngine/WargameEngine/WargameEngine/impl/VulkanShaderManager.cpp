#include "VulkanShaderManager.h"
#include "../Utils.h"
#include "../LogWriter.h"
#include <spirv_glsl.hpp>
#include <algorithm>
#include "VulkanRenderer.h"

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

CVulkanShaderManager::CVulkanShaderManager(CVulkanRenderer & renderer)
	:m_renderer(renderer)
{
}

std::unique_ptr<IShaderProgram> CVulkanShaderManager::NewProgram(std::wstring const& vertex /*= L""*/, std::wstring const& fragment /*= L""*/, std::wstring const& geometry /*= L""*/)
{
	VkDevice device = m_renderer.GetDevice();
	ShaderReflection vertexReflection, fragmentReflection, geometryReflection;
	VkShaderModule vertexShader = CompileShader(vertex, device, &vertexReflection);
	VkShaderModule fragmentShader = CompileShader(fragment, device, &fragmentReflection);
	VkShaderModule geometryShader = CompileShader(geometry, device, &geometryReflection);
	auto program = std::make_unique<CVulkanShaderProgram>(device);
	program->AddShaderModule(vertexShader, VK_SHADER_STAGE_VERTEX_BIT, vertexReflection, m_renderer);
	program->AddShaderModule(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, fragmentReflection, m_renderer);
	program->AddShaderModule(geometryShader, VK_SHADER_STAGE_GEOMETRY_BIT, geometryReflection, m_renderer);

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

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, int elementSize, size_t count, TYPE type, bool perInstance /*= false*/, size_t offset /*= 0*/) const
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

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(size_t size, const void* value) const
{
	auto result = std::make_unique<CStagedVulkanVertexAttribCache>(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_renderer);
	result->Upload(value, size, VK_NULL_HANDLE);
	return std::move(result);
}

void CVulkanShaderManager::DoOnProgramChange(std::function<void(const CVulkanShaderProgram&)> const& handler)
{
	m_onProgramChange = handler;
}

void CVulkanShaderManager::CommitUniforms()
{
	m_programsStack.back()->Commit();
}

void CVulkanShaderManager::FrameEnd()
{
	m_programsStack.back()->FrameEnd();
}

CVulkanShaderProgram::CVulkanShaderProgram(VkDevice device)
	:m_device(device)
{
}

void CVulkanShaderProgram::AddShaderModule(VkShaderModule module, VkShaderStageFlagBits flag, ShaderReflection const& reflection, CVulkanRenderer & renderer)
{
	if (module)
	{
		m_modules.push_back(module);
		m_shaderStageCreateInfos.push_back({ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, flag, module, "main", nullptr });
		m_uniformBuffers.emplace_back();
		m_uniformBuffers.back().cache.resize(reflection.bufferSize * 100);
		m_uniformBuffers.back().buffer = std::make_unique<CVulkanVertexAttribCache>(reflection.bufferSize * 100, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, renderer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
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
			memcpy(buffer.cache.data() + buffer.offset + it->offset, data, size);
			buffer.changed = true;
		}
	}
}

void CVulkanShaderProgram::Commit() const
{
	for (auto& buffer : m_uniformBuffers)
	{
		if (buffer.changed)
		{
			memcpy(buffer.cache.data() + buffer.offset + buffer.reflection.bufferSize, buffer.cache.data() + buffer.offset, buffer.reflection.bufferSize);
			buffer.offset += buffer.reflection.bufferSize;
			buffer.changed = false;
		}
	}
}

void CVulkanShaderProgram::FrameEnd() const
{
	for (auto& buffer : m_uniformBuffers)
	{
		buffer.buffer->Upload(buffer.cache.data(), buffer.offset);
		buffer.offset = 0;
		buffer.changed = false;
	}
}

CVulkanShaderProgram::~CVulkanShaderProgram()
{
	for (VkShaderModule module : m_modules)
	{
		vkDestroyShaderModule(m_device, module, nullptr);
	}
}
