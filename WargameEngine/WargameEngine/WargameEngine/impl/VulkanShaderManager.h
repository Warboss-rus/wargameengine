#pragma once
#include "..\view\IShaderManager.h"
#include <vulkan\vulkan.h>
#include <vector>
#include "VulkanHelpers.h"

class CVulkanShaderProgram : public IShaderProgram
{
public:
	CVulkanShaderProgram(VkDevice device);
	~CVulkanShaderProgram();

	void AddShaderModule(VkShaderModule module, VkShaderStageFlagBits flag);
	const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderInfo() const;
private:
	VkDevice m_device;
	std::vector<VkShaderModule> m_modules;
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
};

class CVulkanVertexAttribCache : public IVertexAttribCache
{
public:
	enum class BufferType
	{
		VERTEX,
		INDEX,
		UNIFORM
	};
	CVulkanVertexAttribCache(size_t size, BufferType type, VkDevice device, VkPhysicalDevice physicalDevice, VkFlags properties, const void * data = nullptr);
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
	CStagedVulkanVertexAttribCache(size_t size, CVulkanVertexAttribCache::BufferType type, VkDevice device, VkPhysicalDevice physicalDevice, const void * data = nullptr);
	void Upload(const void* data, size_t size, VkCommandBuffer commandBuffer);
	size_t GetSize() const { return m_stageBuffer.GetSize(); }
	operator VkBuffer() const { return m_deviceBuffer; }
private:
	CVulkanVertexAttribCache m_deviceBuffer;
	CVulkanVertexAttribCache m_stageBuffer;
};

class CVulkanShaderManager : public IShaderManager
{
public:
	virtual std::unique_ptr<IShaderProgram> NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") override;
	virtual void PushProgram(IShaderProgram const& program) const override;
	virtual void PopProgram() const override;

	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, bool perInstance = false, size_t offset = 0) const override;

	virtual void DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const override;

	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const float* value) const override;
	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const int* value) const override;
	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const unsigned int* value) const override;

	void SetDevice(VkDevice device);
private:
	VkDevice m_device;
	std::unique_ptr<CVulkanShaderProgram> m_defaultProgram;
};