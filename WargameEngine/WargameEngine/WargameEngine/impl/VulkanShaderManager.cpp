#include "VulkanShaderManager.h"
#include "../Utils.h"
#include "../LogWriter.h"
#include <spirv_glsl.hpp>
#include <algorithm>
#include "VulkanRenderer.h"

using namespace wargameEngine;
using namespace view;

namespace
{
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
			reflection.uniforms.push_back({ name, uniformBuffer.name, range.offset, range.range });
			reflection.bufferSize += range.range;
		}
	}
	for (auto& input : resources.stage_inputs)
	{
		uint32_t location = glsl.get_decoration(input.id, spv::DecorationLocation);
		reflection.attributes.push_back({ input.name, location });
	}
	return reflection;
}

VkShaderModule CompileShader(const Path& filename, VkDevice device, ShaderReflection * reflection = nullptr)
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
	LOG_VK_RESULT(result, L"Cannot create shader module for " + to_wstring(filename));
	return shaderModule;
}

VkFormat GetFormat(IShaderManager::Format type, int elementSize)
{
	switch (type)
	{
	case IShaderManager::Format::Float32:
	{
		constexpr VkFormat formats[] = { VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT };
		return formats[elementSize - 1];
	}
	case IShaderManager::Format::SInt32:
	{
		constexpr VkFormat formats[] = { VK_FORMAT_R32_SINT, VK_FORMAT_R32G32_SINT, VK_FORMAT_R32G32B32_SINT, VK_FORMAT_R32G32B32A32_SINT };
		return formats[elementSize - 1];
	}
	case IShaderManager::Format::UInt32:
	{
		constexpr VkFormat formats[] = { VK_FORMAT_R32_UINT, VK_FORMAT_R32G32_UINT, VK_FORMAT_R32G32B32_UINT, VK_FORMAT_R32G32B32A32_UINT };
		return formats[elementSize - 1];
	}
	default:
		return VK_FORMAT_UNDEFINED;
	}
}
}

CVulkanShaderManager::CVulkanShaderManager(CVulkanRenderer & renderer)
	:m_renderer(renderer)
{
}

std::unique_ptr<IShaderProgram> CVulkanShaderManager::NewProgram(const Path& vertex /*= L""*/, const Path& fragment /*= L""*/, const Path& geometry /*= L""*/)
{
	VkDevice device = m_renderer.GetDevice();
	ShaderReflection vertexReflection, fragmentReflection, geometryReflection;
	VkShaderModule vertexShader = CompileShader(vertex, device, &vertexReflection);
	VkShaderModule fragmentShader = CompileShader(fragment, device, &fragmentReflection);
	VkShaderModule geometryShader = CompileShader(geometry, device, &geometryReflection);
	auto program = std::make_unique<CVulkanShaderProgram>(m_renderer);
	program->AddShaderModule(vertexShader, VK_SHADER_STAGE_VERTEX_BIT, vertexReflection);
	program->AddShaderModule(fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, fragmentReflection);
	program->AddShaderModule(geometryShader, VK_SHADER_STAGE_GEOMETRY_BIT, geometryReflection);

	return std::move(program);
}

std::unique_ptr<IShaderProgram> CVulkanShaderManager::NewProgramSource(std::string const& vertex /* = "" */, std::string const& fragment /* = "" */, std::string const& geometry /* = "" */)
{
	return nullptr;
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
	uint32_t location = m_programsStack.back()->GetVertexAttributeLocation(attribute);
	size_t index = m_renderer.GetPipelineHelper().AddVertexAttribute({ location, static_cast<uint32_t>(sizeof(float) * elementSize), GetFormat(Format::Float32, elementSize), perInstance });
	size_t allocationSize = elementSize * count * sizeof(float);
	auto allocation = m_renderer.GetVertexBuffer().Allocate(allocationSize);
	memcpy(std::get<void*>(allocation), values, allocationSize);
	VkBuffer buffer = std::get<0>(allocation);
	VkDeviceSize offset = std::get<1>(allocation);
	vkCmdBindVertexBuffers(m_renderer.GetCommandBuffer(), index, 1, &buffer, &offset);
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance /*= false*/) const
{
	uint32_t location = m_programsStack.back()->GetVertexAttributeLocation(attribute);
	size_t index = m_renderer.GetPipelineHelper().AddVertexAttribute({ location, static_cast<uint32_t>(sizeof(int) * elementSize), GetFormat(Format::SInt32, elementSize), perInstance });
	size_t allocationSize = elementSize * count * sizeof(int);
	auto allocation = m_renderer.GetVertexBuffer().Allocate(allocationSize);
	memcpy(std::get<void*>(allocation), values, allocationSize);
	VkBuffer buffer = std::get<0>(allocation);
	VkDeviceSize offset = std::get<1>(allocation);
	vkCmdBindVertexBuffers(m_renderer.GetCommandBuffer(), index, 1, &buffer, &offset);
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance /*= false*/) const
{
	uint32_t location = m_programsStack.back()->GetVertexAttributeLocation(attribute);
	size_t index = m_renderer.GetPipelineHelper().AddVertexAttribute({ location, static_cast<uint32_t>(sizeof(unsigned int) * elementSize), GetFormat(Format::UInt32, elementSize), perInstance });
	size_t allocationSize = elementSize * count * sizeof(float);
	auto allocation = m_renderer.GetVertexBuffer().Allocate(allocationSize);
	memcpy(std::get<void*>(allocation), values, allocationSize);
	VkBuffer buffer = std::get<0>(allocation);
	VkDeviceSize offset = std::get<1>(allocation);
	vkCmdBindVertexBuffers(m_renderer.GetCommandBuffer(), index, 1, &buffer, &offset);
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, int elementSize, size_t /*count*/, Format type, bool perInstance /*= false*/, size_t offset /*= 0*/) const
{
	uint32_t location = m_programsStack.back()->GetVertexAttributeLocation(attribute);
	size_t index = m_renderer.GetPipelineHelper().AddVertexAttribute({ location, static_cast<uint32_t>(sizeof(unsigned int) * elementSize), GetFormat(type, elementSize), perInstance });
	auto& vulkanCache = reinterpret_cast<const CVulkanVertexAttribCache&>(cache);
	VkBuffer buffer = vulkanCache;
	VkDeviceSize deviceOffset = offset;
	vkCmdBindVertexBuffers(m_renderer.GetCommandBuffer(), index, 1, &buffer, &deviceOffset);
}

void CVulkanShaderManager::DisableVertexAttribute(std::string const& attribute, int /*size*/, const float* /*defaultValue*/) const
{
	uint32_t location = m_programsStack.back()->GetVertexAttributeLocation(attribute);
	m_renderer.GetPipelineHelper().RemoveVertexAttribute(location);
}

void CVulkanShaderManager::DisableVertexAttribute(std::string const& attribute, int /*size*/, const int* /*defaultValue*/) const
{
	uint32_t location = m_programsStack.back()->GetVertexAttributeLocation(attribute);
	m_renderer.GetPipelineHelper().RemoveVertexAttribute(location);
}

void CVulkanShaderManager::DisableVertexAttribute(std::string const& attribute, int /*size*/, const unsigned int* /*defaultValue*/) const
{
	uint32_t location = m_programsStack.back()->GetVertexAttributeLocation(attribute);
	m_renderer.GetPipelineHelper().RemoveVertexAttribute(location);
}

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(size_t size, const void* value) const
{
	auto result = std::make_unique<CVulkanVertexAttribCache>(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_renderer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	result->Upload(value, size);
	return std::move(result);
}

bool CVulkanShaderManager::NeedsMVPMatrix() const
{
	return true;
}

void CVulkanShaderManager::SetMatrices(const float* model /*= nullptr*/, const float* view /*= nullptr*/, const float* projection /*= nullptr*/, const float* mvp /*= nullptr*/, size_t multiviewCount /*= 1*/)
{
	SetUniformValue("model_matrix", 16, 1, model);
	SetUniformValue("proj_matrix", 16, 1, projection);
	SetUniformValue("view_matrix", 16, multiviewCount, view);
	SetUniformValue("mvp_matrix", 16, multiviewCount, mvp);
}

void CVulkanShaderManager::DoOnProgramChange(std::function<void(const CVulkanShaderProgram&)> const& handler)
{
	m_onProgramChange = handler;
}

void CVulkanShaderManager::CommitUniforms(CVulkanSmartBuffer & buffer, bool force)
{
	m_programsStack.back()->Commit(buffer, force);
}

CVulkanShaderProgram::CVulkanShaderProgram(CVulkanRenderer & renderer)
	:m_renderer(renderer)
{
}

void CVulkanShaderProgram::AddShaderModule(VkShaderModule module, VkShaderStageFlagBits flag, ShaderReflection const& reflection)
{
	if (module)
	{
		m_modules.push_back(module);
		m_shaderStageCreateInfos.push_back({ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, flag, module, "main", nullptr });
		m_uniformBuffers.emplace_back();
		m_uniformBuffers.back().cache.resize(reflection.bufferSize);
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
			buffer.changed = true;
		}
	}
}

void CVulkanShaderProgram::Commit(CVulkanSmartBuffer & smartBuffer, bool force) const
{
	for (auto& buffer : m_uniformBuffers)
	{
		if (buffer.changed || force)
		{
			void* data = nullptr;
			std::tie(buffer.buffer, buffer.offset, data) = smartBuffer.Allocate(buffer.cache.size());
			memcpy(data, buffer.cache.data(), buffer.cache.size());
			buffer.changed = false;
		}
	}
}

uint32_t CVulkanShaderProgram::GetVertexAttributeLocation(std::string const& name) const
{
	for (auto& mod : m_uniformBuffers)
	{
		for (auto& attrib : mod.reflection.attributes)
		{
			if (attrib.name == name)
			{
				return attrib.location;
			}
		}
	}
	return static_cast<uint32_t>(-1);
}

CVulkanShaderProgram::~CVulkanShaderProgram()
{
	for (VkShaderModule module : m_modules)
	{
		vkDestroyShaderModule(m_renderer.GetDevice(), module, nullptr);
	}
}