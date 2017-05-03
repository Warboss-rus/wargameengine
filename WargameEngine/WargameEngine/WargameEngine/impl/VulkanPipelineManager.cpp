#include "VulkanPipelineManager.h"
#include "VulkanRenderer.h"
#include "VulkanShaderManager.h"
#include <algorithm>

void CVulkanPipelineManager::Init(VkDevice device)
{
	m_pipelineLayout.Destroy();
	const VkResult result = vkCreatePipelineLayout(device, &layout_create_info, nullptr, &m_pipelineLayout);
	m_pipelineLayout.SetDevice(device);
	pipeline_create_info.layout = m_pipelineLayout;
	CHECK_VK_RESULT(result, "Cannot create pipeline layout");
	m_device = device;
}

VkPipeline CVulkanPipelineManager::GetPipeline()
{
	auto it = m_pipelines.find(m_currentKey);
	if (it == m_pipelines.end())
	{
		VkPipeline pipeline;
		const VkResult result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline);
		CHECK_VK_RESULT(result, "Cannot create pipeline");
		it = m_pipelines.emplace(std::make_pair(m_currentKey, pipeline)).first;
	}
	return it->second;
}

void CVulkanPipelineManager::Bind(VkCommandBuffer commandBuffer)
{
	const VkPipeline newPipeline = GetPipeline();
	if (newPipeline != m_currentPipeline || commandBuffer != m_currentBuffer)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, newPipeline);
		m_currentPipeline = newPipeline;
		m_currentBuffer = commandBuffer;
	}
}

void CVulkanPipelineManager::SetShaderProgram(CVulkanShaderProgram const& program)
{
	pipeline_create_info.stageCount = program.GetShaderInfo().size();
	pipeline_create_info.pStages = program.GetShaderInfo().data();
	m_currentKey.program = &program;
}

void CVulkanPipelineManager::SetVertexAttributes(std::vector<VertexAttrib> const& attribs)
{
	vertex_binding_descriptions.clear();
	vertex_attribute_descriptions.clear();
	for (auto& attrib : attribs)
	{
		vertex_binding_descriptions.push_back({ static_cast<uint32_t>(vertex_binding_descriptions.size()), attrib.size, attrib.perInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX });
		vertex_attribute_descriptions.push_back({ attrib.pos, vertex_binding_descriptions.back().binding, attrib.format, 0 });
	}
	vertex_input_state_create_info.vertexBindingDescriptionCount = vertex_binding_descriptions.size();
	vertex_input_state_create_info.pVertexBindingDescriptions = vertex_binding_descriptions.data();
	vertex_input_state_create_info.vertexAttributeDescriptionCount = vertex_attribute_descriptions.size();
	vertex_input_state_create_info.pVertexAttributeDescriptions = vertex_attribute_descriptions.data();
	m_currentKey.attribs = attribs;
}

size_t CVulkanPipelineManager::AddVertexAttribute(VertexAttrib attrib)
{
	auto attribs = m_currentKey.attribs;
	auto it = std::find_if(attribs.begin(), attribs.end(), [&attrib](const VertexAttrib& attr) {
		return (attr.pos == attrib.pos) && (attr.perInstance == attrib.perInstance);
	});
	if (it != attribs.end())
	{
		return it - attribs.begin();
	}
	else
	{
		attribs.push_back(attrib);
		SetVertexAttributes(attribs);
		return attribs.size() - 1;
	}
}

void CVulkanPipelineManager::RemoveVertexAttribute(uint32_t pos)
{
	auto attribs = m_currentKey.attribs;
	attribs.erase(std::remove_if(attribs.begin(), attribs.end(), [pos](const VertexAttrib& attrib) { return attrib.pos == pos; }), attribs.end());
	SetVertexAttributes(attribs);
}

void CVulkanPipelineManager::SetDescriptorLayout(const VkDescriptorSetLayout* layouts, uint32_t count)
{
	layout_create_info.setLayoutCount = count;
	layout_create_info.pSetLayouts = layouts;
	m_currentKey.descriptors = layouts[0];
}

void CVulkanPipelineManager::SetTopology(VkPrimitiveTopology topology)
{
	input_assembly_state_create_info.topology = topology;
	m_currentKey.topology = topology;
}

void CVulkanPipelineManager::SetBlending(bool enable)
{
	color_blend_attachment_state.blendEnable = enable ? VK_TRUE : VK_FALSE;
	m_currentKey.blending = enable;
}

void CVulkanPipelineManager::SetDepthParams(bool test, bool write)
{
	depthStencil.depthTestEnable = test ? VK_TRUE : VK_FALSE;
	depthStencil.depthWriteEnable = write ? VK_TRUE : VK_FALSE;
	m_currentKey.depthTest = test;
	m_currentKey.depthWrite = test;
}

void CVulkanPipelineManager::SetRenderPass(VkRenderPass pass)
{
	pipeline_create_info.renderPass = pass;
	m_currentKey.renderPass = pass;
}

void CVulkanPipelineManager::Destroy()
{
	for (auto& pair : m_pipelines)
	{
		vkDestroyPipeline(m_device, pair.second, nullptr);
	}
	m_pipelines.clear();
	m_pipelineLayout.Destroy();
}

void CVulkanDescriptorSetManager::Init(VkDevice device, uint32_t poolSize)
{
	m_device = device;

	VkDescriptorSetLayoutBinding layout_bindings[] = {
		{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr },
		{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
		{ 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
	};
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, nullptr, 0, 2, layout_bindings };
	VkResult result = vkCreateDescriptorSetLayout(m_device, &descriptor_set_layout_create_info, nullptr, &m_programDescriptorSetLayout);
	CHECK_VK_RESULT(result, "cannot create descriptor set layout");
	m_programDescriptorSetLayout.SetDevice(m_device);
	descriptor_set_layout_create_info.bindingCount = 1;
	descriptor_set_layout_create_info.pBindings = &layout_bindings[2];
	result = vkCreateDescriptorSetLayout(m_device, &descriptor_set_layout_create_info, nullptr, &m_textureDescriptorSetLayout);
	CHECK_VK_RESULT(result, "cannot create descriptor set layout");
	m_textureDescriptorSetLayout.SetDevice(m_device);
	CreatePool(poolSize);
}

void CVulkanDescriptorSetManager::SetShaderProgram(const CVulkanShaderProgram* program, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
	ProgramDescriptorSetKey key = { program, program->GetVertexAttribBuffer(), program->GetFragmentAttribBuffer() };
	auto it = m_programDescriptorSets.find(key);
	if (it == m_programDescriptorSets.end())
	{
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, m_desciptorPool, 1, &m_programDescriptorSetLayout };
		VkDescriptorSet descriptorSet;
		VkResult result = vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, &descriptorSet);
		if (result != VK_SUCCESS)
		{
			CreatePool(m_poolSize * 2);
			result = vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, &descriptorSet);
		}
		CHECK_VK_RESULT(result, "Cannot allocate descriptor set");
		VkDescriptorBufferInfo bufferInfos[] = {
			{ program->GetVertexAttribBuffer(), 0, program->GetVertexBufferRange() },
			{ program->GetFragmentAttribBuffer(), 0, program->GetFragmentBufferRange() },
		};
		VkWriteDescriptorSet descriptorWrites[] = {
			{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, descriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, nullptr, &bufferInfos[0], nullptr },
			{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, descriptorSet, 1, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, nullptr, &bufferInfos[1], nullptr },
		};
		vkUpdateDescriptorSets(m_device, sizeof(descriptorWrites) / sizeof(descriptorWrites[0]), descriptorWrites, 0, nullptr);
		it = m_programDescriptorSets.emplace(std::make_pair(key, descriptorSet)).first;
	}
	m_currentProgramDescriptorSet = it->second;
	if (commandBuffer)
	{
		uint32_t offsets[] = { program->GetVertexAttribOffset(), program->GetFragmentAttribOffset() };
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &m_currentProgramDescriptorSet, sizeof(offsets) / sizeof(offsets[0]), offsets);
	}
}

void CVulkanDescriptorSetManager::SetTexture(const CVulkanCachedTexture* texture, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
	auto it = m_textureSets.find(texture);
	if (it == m_textureSets.end())
	{
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, m_desciptorPool, 1, &m_textureDescriptorSetLayout };
		VkDescriptorSet descriptorSet;
		VkResult result = vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, &descriptorSet);
		if (result != VK_SUCCESS)
		{
			CreatePool(m_poolSize * 2);
			result = vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, &descriptorSet);
		}
		CHECK_VK_RESULT(result, "Cannot allocate descriptor set");
		VkDescriptorImageInfo image_info = { texture->GetSampler(), texture->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkWriteDescriptorSet descriptorWrites[] = {
			{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, descriptorSet, 2, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &image_info, nullptr, nullptr },
		};
		vkUpdateDescriptorSets(m_device, sizeof(descriptorWrites) / sizeof(descriptorWrites[0]), descriptorWrites, 0, nullptr);
		it = m_textureSets.emplace(std::make_pair(texture, descriptorSet)).first;
	}
	m_currentTextureDescriptorSet = it->second;
	if (commandBuffer)
	{
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &m_currentTextureDescriptorSet, 0, nullptr);
	}
}

void CVulkanDescriptorSetManager::BindAll(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
	VkDescriptorSet sets[] = { m_currentProgramDescriptorSet, m_currentTextureDescriptorSet };
	uint32_t offsets[] = { 0, 0 };
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, sizeof(sets) / sizeof(sets[0]), sets, sizeof(offsets) / sizeof(offsets[0]), offsets);
}

VkDescriptorSet CVulkanDescriptorSetManager::GetTextureDescriptor(const CVulkanCachedTexture* texture)
{
	auto it = m_textureSets.find(texture);
	return it == m_textureSets.end() ? VK_NULL_HANDLE : it->second;
}

void CVulkanDescriptorSetManager::DeleteSet(VkDescriptorSet set)
{
	vkFreeDescriptorSets(m_device, m_desciptorPool, 1, &set);
	auto it = std::find_if(m_textureSets.begin(), m_textureSets.end(), [set](std::pair<const CVulkanCachedTexture*, VkDescriptorSet> const& pair) { return pair.second == set; });
	if (it != m_textureSets.end())
		m_textureSets.erase(it);
	auto it2 = std::find_if(m_programDescriptorSets.begin(), m_programDescriptorSets.end(), [set](std::pair<ProgramDescriptorSetKey, VkDescriptorSet> const& pair) { return pair.second == set; });
	if (it2 != m_programDescriptorSets.end())
		m_programDescriptorSets.erase(it2);
}

std::vector<VkDescriptorSet> CVulkanDescriptorSetManager::GetSetsWithUniformBuffer(VkBuffer buffer) const
{
	std::vector<VkDescriptorSet> result;
	for (auto& set : m_programDescriptorSets)
	{
		if (set.first.vertexBuffer == buffer || set.first.fragmentBuffer == buffer)
		{
			result.push_back(set.second);
		}
	}
	return result;
}

void CVulkanDescriptorSetManager::CreatePool(uint32_t poolSize)
{
	m_poolSize = poolSize;
	VkDescriptorPoolSize pool_sizes[] = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, poolSize },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, poolSize },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, poolSize },
	};
	VkDescriptorPoolCreateInfo descriptor_pool_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, poolSize, sizeof(pool_sizes) / sizeof(pool_sizes[0]), pool_sizes };
	VkResult result = vkCreateDescriptorPool(m_device, &descriptor_pool_create_info, nullptr, &m_desciptorPool);
	CHECK_VK_RESULT(result, "cannot create descriptor pool");
	m_desciptorPool.SetDevice(m_device);
}