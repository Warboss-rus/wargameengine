#pragma once
#include "VulkanHelpers.h"
#include <map>
#include <vector>
#include <vulkan/vulkan.h>

class CVulkanShaderProgram;
class CVulkanCachedTexture;

class CVulkanPipelineManager
{
public:
	~CVulkanPipelineManager() { Destroy(); }
	void SetShaderProgram(CVulkanShaderProgram const& program);
	struct VertexAttrib
	{
		uint32_t pos;
		uint32_t size;
		VkFormat format;
		bool perInstance;
		bool operator<(VertexAttrib const& other) const { return std::tie(pos, size, format, perInstance) < std::tie(other.pos, other.size, other.format, other.perInstance); }
	};
	void SetVertexAttributes(std::vector<VertexAttrib> const& attribs);
	size_t AddVertexAttribute(VertexAttrib attrib);
	void RemoveVertexAttribute(uint32_t pos);
	void SetDescriptorLayout(const VkDescriptorSetLayout* layouts, uint32_t count = 1);
	void SetTopology(VkPrimitiveTopology topology);
	void SetBlending(bool enable);
	void SetDepthParams(bool test, bool write);
	void SetColorWriteParams(bool rgb, bool a);
	void SetRenderPass(VkRenderPass pass);
	void Destroy();
	void Init(VkDevice device);
	VkPipeline GetPipeline();
	VkPipelineLayout GetLayout() const { return m_pipelineLayout; }
	void Bind(VkCommandBuffer commandBuffer);

private:
	CHandleWrapper<VkPipelineLayout, vkDestroyPipelineLayout> m_pipelineLayout;
	std::vector<VkDynamicState> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR,
	};
	VkPipelineViewportStateCreateInfo viewport_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, nullptr, 0, 1, nullptr, 1, nullptr };
	std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions;
	std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions;
	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, nullptr, 0, 0, nullptr, 0, nullptr };
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, nullptr, 0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE };
	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, nullptr, 0, VK_FALSE, VK_FALSE,
		VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f };
	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, nullptr, 0, VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE };
	VkPipelineColorBlendAttachmentState color_blend_attachment_state = { VK_FALSE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_DST_ALPHA, VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT };
	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, nullptr, 0, VK_FALSE, VK_LOGIC_OP_COPY, 1, &color_blend_attachment_state, { 0.0f, 0.0f, 0.0f, 0.0f } };
	VkPipelineDepthStencilStateCreateInfo depthStencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, nullptr, 0, VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE, {}, {}, 0.0f, 1.0f };
	VkPipelineLayoutCreateInfo layout_create_info = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, nullptr, 0, 0, nullptr, 0, nullptr };
	VkPipelineDynamicStateCreateInfo dynamic_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, nullptr, 0, static_cast<uint32_t>(dynamic_states.size()), dynamic_states.data() };
	VkGraphicsPipelineCreateInfo pipeline_create_info = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0, 0 /*static_cast<uint32_t>(shader_stage_create_infos.size())*/, nullptr /*shader_stage_create_infos.data()*/,
		&vertex_input_state_create_info, &input_assembly_state_create_info, nullptr, &viewport_state_create_info, &rasterization_state_create_info, &multisample_state_create_info, &depthStencil, &color_blend_state_create_info,
		&dynamic_state_create_info, m_pipelineLayout, VK_NULL_HANDLE /*m_renderPass*/, 0, VK_NULL_HANDLE, -1 };

	VkDevice m_device;
	struct Key
	{
		const CVulkanShaderProgram* program;
		VkDescriptorSetLayout descriptors;
		VkRenderPass renderPass;
		VkPrimitiveTopology topology;
		std::vector<VertexAttrib> attribs;
		bool blending = false;
		bool depthTest = false;
		bool depthWrite = false;
		VkColorComponentFlags colorWrite = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		bool operator<(Key const& other) const { return std::tie(program, descriptors, renderPass, topology, attribs, blending, depthTest, depthWrite, colorWrite) 
			< std::tie(other.program, other.descriptors, other.renderPass, other.topology, other.attribs, other.blending, other.depthTest, other.depthWrite, other.colorWrite); }
	};
	Key m_currentKey;
	std::map<Key, VkPipeline> m_pipelines;
	VkPipeline m_currentPipeline = VK_NULL_HANDLE;
	VkCommandBuffer m_currentBuffer = VK_NULL_HANDLE;
};

class CVulkanDescriptorSetManager
{
public:
	void Init(VkDevice device, uint32_t poolSize);
	void SetShaderProgram(const CVulkanShaderProgram* program, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
	void SetTexture(const CVulkanCachedTexture* texture, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
	void BindAll(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
	VkDescriptorSetLayout GetProgramLayout() { return m_programDescriptorSetLayout; }
	VkDescriptorSetLayout GetTextureLayout() { return m_textureDescriptorSetLayout; }
	VkDescriptorSet GetTextureDescriptor(const CVulkanCachedTexture* texture);
	void DeleteSet(VkDescriptorSet set);
	std::vector<VkDescriptorSet> GetSetsWithUniformBuffer(VkBuffer buffer) const;

private:
	void CreatePool(uint32_t poolSize);
	CHandleWrapper<VkDescriptorSetLayout, vkDestroyDescriptorSetLayout> m_programDescriptorSetLayout;
	CHandleWrapper<VkDescriptorSetLayout, vkDestroyDescriptorSetLayout> m_textureDescriptorSetLayout;
	CHandleWrapper<VkDescriptorPool, vkDestroyDescriptorPool> m_desciptorPool;
	struct ProgramDescriptorSetKey
	{
		const CVulkanShaderProgram* program;
		VkBuffer vertexBuffer;
		VkBuffer fragmentBuffer;
		bool operator<(const ProgramDescriptorSetKey& other) const { return std::tie(program, vertexBuffer, fragmentBuffer) < std::tie(other.program, other.vertexBuffer, other.fragmentBuffer); }
	};
	std::map<ProgramDescriptorSetKey, VkDescriptorSet> m_programDescriptorSets;
	std::map<const CVulkanCachedTexture*, VkDescriptorSet> m_textureSets;
	VkDevice m_device;
	uint32_t m_poolSize;
	VkDescriptorSet m_currentProgramDescriptorSet;
	VkDescriptorSet m_currentTextureDescriptorSet;
};