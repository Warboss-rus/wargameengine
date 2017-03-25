#pragma once
#include "..\view\IShaderManager.h"
#include <vulkan\vulkan.h>
#include <vector>
#include <functional>
#include "VulkanHelpers.h"
#include "VulkanMemory.h"

struct ShaderReflection
{
	struct UniformDescription
	{
		std::string name;
		std::string bufferName;
		size_t offset;
		size_t size;
	};
	struct AttributeDescription
	{
		std::string name;
		uint32_t location;
	};
	std::vector<UniformDescription> uniforms;
	std::vector<AttributeDescription> attributes;
	size_t bufferSize;
};

struct UniformBufferWrapper
{
	mutable std::unique_ptr<CVulkanVertexAttribCache> buffer;
	ShaderReflection reflection;
	std::vector<char> cache;
	size_t offset = 0;
	bool changed = false;
};

class CVulkanShaderProgram : public IShaderProgram
{
public:
	CVulkanShaderProgram(CVulkanRenderer & renderer);
	~CVulkanShaderProgram();

	void AddShaderModule(VkShaderModule module, VkShaderStageFlagBits flag, ShaderReflection const& reflection);
	const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderInfo() const;
	VkBuffer GetVertexAttribBuffer() const { return *m_uniformBuffers[0].buffer; }
	VkBuffer GetFragmentAttribBuffer() const { return *m_uniformBuffers[1].buffer; }
	size_t GetVertexAttribBufferSize() const { return m_uniformBuffers[0].cache.size(); }
	size_t GetFragmentAttribBufferSize() const { return m_uniformBuffers[1].cache.size(); }
	size_t GetVertexAttribOffset() const { return m_uniformBuffers[0].offset == 0 ? m_uniformBuffers[0].offset : m_uniformBuffers[0].offset - m_uniformBuffers[0].reflection.bufferSize; }
	size_t GetFragmentAttribOffset() const { return m_uniformBuffers[1].offset == 0 ? m_uniformBuffers[1].offset : m_uniformBuffers[1].offset - m_uniformBuffers[1].reflection.bufferSize; }
	size_t GetVertexBufferRange() const { return m_uniformBuffers[0].reflection.bufferSize; }
	size_t GetFragmentBufferRange() const { return m_uniformBuffers[1].reflection.bufferSize; }
	void SetUniformValue(std::string const& name, const void * data, size_t size) const;
	void Commit() const;
	void FrameEnd() const;
	uint32_t GetVertexAttributeLocation(std::string const& name) const;
private:
	CVulkanRenderer & m_renderer;
	std::vector<VkShaderModule> m_modules;
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
	mutable std::vector<UniformBufferWrapper> m_uniformBuffers;
	std::vector<char> m_fragmentAttribCache;
};

class CVulkanShaderManager : public IShaderManager
{
public:
	CVulkanShaderManager(CVulkanRenderer & renderer);
	virtual std::unique_ptr<IShaderProgram> NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") override;
	virtual void PushProgram(IShaderProgram const& program) const override;
	virtual void PopProgram() const override;

	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, int elementSize, size_t count, TYPE type, bool perInstance = false, size_t offset = 0) const override;

	virtual void DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const override;

	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(size_t size, const void* value) const override;

	void DoOnProgramChange(std::function<void(const CVulkanShaderProgram&)> const& handler);
	void CommitUniforms();
	void FrameEnd() const;
	const CVulkanShaderProgram* GetActiveProgram() const { return m_programsStack.back(); }
private:
	CVulkanRenderer & m_renderer;
	std::unique_ptr<CVulkanShaderProgram> m_defaultProgram;
	std::function<void(const CVulkanShaderProgram&)> m_onProgramChange;
	mutable std::vector<const CVulkanShaderProgram*> m_programsStack;
	
};