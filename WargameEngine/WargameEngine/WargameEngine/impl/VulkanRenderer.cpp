#include "VulkanRenderer.h"
#include "..\LogWriter.h"
#include <iterator>
#include <algorithm>
#include "..\view\TextureManager.h"
#include "..\view\IViewport.h"

namespace
{
static const uint32_t COMMAND_BUFFERS_COUNT = 3;
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#ifndef clamp
#define clamp(value, minV, maxV) value = min(max(value, minV), maxV)
#endif

bool CheckPhysicalDevice(VkPhysicalDevice device, uint32_t & queue_family_index)
{
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures   features;
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);
	uint32_t major_version = VK_VERSION_MAJOR(properties.apiVersion);
	if ((major_version < 1) && (properties.limits.maxImageDimension2D < 4096))
	{
		return false;
	}
	uint32_t count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
	if (count == 0)
	{
		return false;
	}
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, &queueFamilyProperties[0]);
	for (uint32_t i = 0; i < count; ++i)
	{
		if ((queueFamilyProperties[i].queueCount > 0) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			queue_family_index = i;
			return true;
		}
	}

	return false;
}
VkExtent2D GetSurfaceExtent(VkSurfaceCapabilitiesKHR const& capabilities)
{
	if (capabilities.currentExtent.width == -1)
	{
		VkExtent2D swap_chain_extent = { 640, 480 };
		swap_chain_extent.width = clamp(swap_chain_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		swap_chain_extent.height = clamp(swap_chain_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return swap_chain_extent;
	}
	return capabilities.currentExtent;
}

VkSurfaceFormatKHR GetSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & formats)
{
	if ((formats.size() == 1) && (formats[0].format == VK_FORMAT_UNDEFINED))
	{
		return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}
	for (const VkSurfaceFormatKHR &format : formats)
	{
		if (format.format == VK_FORMAT_R8G8B8A8_UNORM)
		{
			return format;
		}
	}
	return formats.front();
}

VkPresentModeKHR SelectPresentMode(const std::vector<VkPresentModeKHR> & supportedPresentModes, const std::vector<VkPresentModeKHR> & desiredPresentModes)
{
	for (VkPresentModeKHR desiredMode : desiredPresentModes)
	{
		if (std::find(supportedPresentModes.begin(), supportedPresentModes.end(), desiredMode) != supportedPresentModes.end())
		{
			return desiredMode;
		}
	}
	return supportedPresentModes.front();
}

void UpdateBuffer(std::unique_ptr<CVulkanVertexAttribCache> & buffer, VkDevice device, VkPhysicalDevice physicalDevice, size_t size, const void * data)
{
	if (!buffer || buffer->GetSize() < size)
	{
		buffer.reset(new CVulkanVertexAttribCache(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, device, physicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, data));
	}
	else
	{
		buffer->Upload(data, size);
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) 
{
	LogWriter::WriteLine(msg);
	return VK_FALSE;
}
}

CVulkanRenderer::CVulkanRenderer(const std::vector<const char*> & instanceExtensions)
{
	VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "WargameEngine", VK_MAKE_VERSION(1, 0, 0), "WargameEngine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0 };
	const std::vector<const char*> validationLayers = {
#ifdef _DEBUG
		"VK_LAYER_LUNARG_standard_validation"
#endif
	};
	VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &appInfo, validationLayers.size(), validationLayers.data(), static_cast<uint32_t>(instanceExtensions.size()), instanceExtensions.data() };
	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_instance);
	CHECK_VK_RESULT(result, "Cannot create vulkan instance");
#ifdef _DEBUG
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = DebugCallback;
	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
	CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
	result = CreateDebugReportCallback(m_instance, &createInfo, nullptr, &m_debugCallback);
	m_debugCallbackDestructor.SetDestructorFunction([this] {
		PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;
		DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");
		DestroyDebugReportCallback(m_instance, m_debugCallback, nullptr);
	});
#endif

	CreateDeviceAndQueues();
	
	m_commandPool.SetDevice(m_device);
	m_renderPass.SetDevice(m_device);
	m_shaderManager.SetDevice(m_device, m_physicalDevice);

	CreateDescriptors();

	m_emptyTexture = std::make_unique<CVulkanCachedTexture>(m_device, m_descriptorSet);
	m_emptyTexture->Init(1, 1, m_physicalDevice);
	m_emptyTexture->Bind();

	float zero = 0.0f;
	m_emptyBuffer = std::make_unique<CVulkanVertexAttribCache>(sizeof(float), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, m_device, m_physicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &zero);

	m_shaderManager.DoOnProgramChange([this](const CVulkanShaderProgram & program) {
		VkDescriptorBufferInfo bufferInfos[] = {
			{ program.GetVertexAttribBuffer(), 0, program.GetVertexAttribBufferSize() },
			{ program.GetFragmentAttribBuffer(), 0, program.GetFragmentAttribBufferSize() },
		};
		VkDescriptorImageInfo image_info = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkWriteDescriptorSet descriptorWrites[] = {
			{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_descriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &bufferInfos[0], nullptr },
			{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_descriptorSet, 1, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &bufferInfos[1], nullptr },
		};
		vkUpdateDescriptorSets(m_device, sizeof(descriptorWrites) / sizeof(descriptorWrites[0]), descriptorWrites, 0, nullptr);
	});
}

CVulkanRenderer::~CVulkanRenderer()
{
	if (m_device)
	{
		vkDeviceWaitIdle(m_device);
	}
}

VkInstance CVulkanRenderer::GetInstance() const
{
	return m_instance;
}

void CVulkanRenderer::SetSurface(VkSurfaceKHR surface)
{
	m_surface = surface;
	m_surfaceDestructor.SetDestructorFunction([this] {vkDestroySurfaceKHR(m_instance, m_surface, nullptr); });
	CreateSwapchain();
	CreateCommandBuffers();
	CreateRenderPass();
	m_defaultProgram = m_shaderManager.NewProgram(L"Killteam/shaders/Vulkan/vert.spv", L"Killteam/shaders/Vulkan/frag.spv");
	m_shaderManager.PushProgram(*m_defaultProgram);
	m_pipelineHelper.SetShaderProgram(*reinterpret_cast<CVulkanShaderProgram*>(m_defaultProgram.get()));
	m_pipelineHelper.SetVertexAttributes({
	{ 0, sizeof(float) * 3, VK_FORMAT_R32G32B32_SFLOAT, false },
	{ 1, sizeof(float) * 3, VK_FORMAT_R32G32B32_SFLOAT, false },
	{ 2, sizeof(float) * 2, VK_FORMAT_R32G32_SFLOAT, false },
	});
	m_pipelineHelper.SetDescriptorLayout(&m_descriptorSetLayout, 1);
	m_pipelineHelper.Init(m_device, m_renderPass);
}

void CVulkanRenderer::AcquireImage()
{
	m_currentCommandBufferIndex = (m_currentCommandBufferIndex + 1) % COMMAND_BUFFERS_COUNT;
	auto& commandBuffer = m_commandBuffers[m_currentCommandBufferIndex];
	commandBuffer.WaitFence();
	
	VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, commandBuffer.GetImageAvailibleSemaphore(), VK_NULL_HANDLE, &m_currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return CreateSwapchain();
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		LogWriter::WriteLine("Cannot acqure image");
	}

	m_currentImage = m_swapchain.GetImages()[m_currentImageIndex];
	InitFramebuffer();
	const VkCommandBufferBeginInfo beginBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr };
	result = vkBeginCommandBuffer(commandBuffer, &beginBufferInfo);
	LOG_VK_RESULT(result, L"cannot begin command buffer");

	if (m_presentQueue != m_graphicsQueue)
	{
		const VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		VkImageMemoryBarrier barrierFromPresentToDraw = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, m_presentQueueFamilyIndex, m_graphicsQueueFamilyIndex, m_currentImage, imageSubresourceRange };
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromPresentToDraw);
	}
}

void CVulkanRenderer::Present()
{
	auto& commandBuffer = m_commandBuffers[m_currentCommandBufferIndex];
	vkCmdEndRenderPass(commandBuffer);

	if (m_graphicsQueue != m_presentQueue)
	{
		const VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		VkImageMemoryBarrier barrierFromDrawToPresent = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, m_graphicsQueueFamilyIndex, m_presentQueueFamilyIndex, m_currentImage, imageSubresourceRange };
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromDrawToPresent);
	}

	VkResult result = vkEndCommandBuffer(commandBuffer);
	LOG_VK_RESULT(result, L"cannot end command buffer");

	VkCommandBuffer bufferHandle = commandBuffer;
	VkSemaphore imageAvailibleSemaphore = commandBuffer.GetImageAvailibleSemaphore();
	VkSemaphore renderingFinishedSemaphore = commandBuffer.GetRenderingFinishedSemaphore();
	VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 1, &imageAvailibleSemaphore, &mask, 1, &bufferHandle, 1, &renderingFinishedSemaphore };
	result = vkQueueSubmit(m_presentQueue, 1, &submit_info, commandBuffer.GetFence());
	LOG_VK_RESULT(result, "Cannot submit buffer");

	VkSwapchainKHR swapchain = m_swapchain;
	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, nullptr, 1, &renderingFinishedSemaphore, 1, &swapchain, &m_currentImageIndex, nullptr };
	result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return CreateSwapchain();
	}
	LOG_VK_RESULT(result, L"Present failed");
}

void CVulkanRenderer::EnableMultisampling(bool enable)
{
}

void CVulkanRenderer::WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const
{
	m_matrixManager.WindowCoordsToWorldVector(x, y, (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight(), viewport.GetViewMatrix(), viewport.GetProjectionMatrix(), start, end);
}

void CVulkanRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	m_matrixManager.WorldCoordsToWindowCoords(worldCoords, (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight(), viewport.GetViewMatrix(), viewport.GetProjectionMatrix(), x, y);
}

std::unique_ptr<IFrameBuffer> CVulkanRenderer::CreateFramebuffer() const
{
	return nullptr;
}

void CVulkanRenderer::SetNumberOfLights(size_t count)
{
	static const std::string numberOfLightsKey = "lightsCount";
	int number = static_cast<int>(count);
	m_shaderManager.SetUniformValue(numberOfLightsKey, 1, 1, &number);
}

void CVulkanRenderer::SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular)
{
	const std::string key = "lights[" + std::to_string(index) + "].";
	m_shaderManager.SetUniformValue(key + "pos", 3, 1, position.ptr());
	m_shaderManager.SetUniformValue(key + "ambient", 4, 1, ambient);
	m_shaderManager.SetUniformValue(key + "diffuse", 4, 1, diffuse);
	m_shaderManager.SetUniformValue(key + "specular", 4, 1, specular);
}

float CVulkanRenderer::GetMaximumAnisotropyLevel() const
{
	return 16.0f;
}

void CVulkanRenderer::GetProjectionMatrix(float * matrix) const
{
	m_matrixManager.GetProjectionMatrix(matrix);
}

void CVulkanRenderer::EnableDepthTest(bool enable)
{
}

void CVulkanRenderer::EnableBlending(bool enable)
{
}

void CVulkanRenderer::SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane /*= 1.0f*/, float farPane /*= 1000.0f*/)
{
	m_viewport = { static_cast<float>(viewportX), static_cast<float>(viewportY), static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), nearPane, farPane };
	VkRect2D scissor = { {static_cast<int32_t>(viewportX), static_cast<int32_t>(viewportY)}, {viewportWidth, viewportHeight} };
	vkCmdSetViewport(m_commandBuffers[m_currentCommandBufferIndex], 0, 1, &m_viewport);
	vkCmdSetScissor(m_commandBuffers[m_currentCommandBufferIndex], 0, 1, &scissor);
	m_matrixManager.SetUpViewport(viewportWidth, viewportHeight, viewingAngle, nearPane, farPane);
}

void CVulkanRenderer::DrawIn2D(std::function<void() > const& drawHandler)
{
	m_matrixManager.SaveMatrices();
	m_matrixManager.SetOrthographicProjection(m_viewport.x, m_viewport.x + m_viewport.width, m_viewport.y, m_viewport.y + m_viewport.height);
	m_matrixManager.ResetModelView();
	drawHandler();
	m_matrixManager.RestoreMatrices();
}

void CVulkanRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
}

void CVulkanRenderer::ClearBuffers(bool color /*= true*/, bool depth /*= true*/)
{
	const VkImageSubresourceRange imageRange = { static_cast<VkFlags>((color ? VK_IMAGE_ASPECT_COLOR_BIT : 0) | (depth ? VK_IMAGE_ASPECT_DEPTH_BIT : 0)), 0, 1, 0, 1 };
	VkClearColorValue clearColor = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	auto& commandBuffer = m_commandBuffers[m_currentCommandBufferIndex];
	vkCmdClearColorImage(commandBuffer, m_currentImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &imageRange);

	VkRenderPassBeginInfo render_pass_begin_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, nullptr, m_renderPass, m_frameBuffers[m_currentCommandBufferIndex],{ { 0, 0 }, m_swapchain.GetExtent() }, 0, nullptr };
	vkCmdBeginRenderPass(commandBuffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineHelper.GetPipeline());

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineHelper.GetLayout(), 0, 1, &m_descriptorSet, 0, nullptr);
}

void CVulkanRenderer::SetTextureManager(CTextureManager & textureManager)
{
	m_textureManager = &textureManager;
}

void CVulkanRenderer::ActivateTextureSlot(TextureSlot slot)
{
}

void CVulkanRenderer::UnbindTexture()
{
	m_emptyTexture->Bind();
}

std::unique_ptr<ICachedTexture> CVulkanRenderer::CreateEmptyTexture(bool cubemap /*= false*/)
{
	return std::make_unique<CVulkanCachedTexture>(m_device, m_descriptorSet);
}

void CVulkanRenderer::SetTextureAnisotropy(float value /*= 1.0f*/)
{
}

void CVulkanRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
	m_serviceCommandBuffer->WaitFence();
	auto& vulkanTexture = reinterpret_cast<CVulkanCachedTexture&>(texture);
	vulkanTexture.Init(width, height, m_physicalDevice);
	vulkanTexture.Upload(data, *m_serviceCommandBuffer);

	//SubmitServiceCommandBuffer();
}

void CVulkanRenderer::UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
}

void CVulkanRenderer::UploadCubemap(ICachedTexture & texture, TextureMipMaps const& sides, unsigned short bpp, int flags)
{
}

bool CVulkanRenderer::Force32Bits() const
{
	return true;
}

bool CVulkanRenderer::ForceFlipBMP() const
{
	return false;
}

bool CVulkanRenderer::ConvertBgra() const
{
	return true;
}

void CVulkanRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	if (!vertices.empty()) UpdateBuffer(m_vertexBuffer, m_device, m_physicalDevice, vertices.size() * sizeof(CVector3f), vertices.data()->ptr());
	if (!normals.empty()) UpdateBuffer(m_normalsBuffer, m_device, m_physicalDevice, normals.size() * sizeof(CVector3f), normals.data());
	if (!texCoords.empty()) UpdateBuffer(m_texCoordBuffer, m_device, m_physicalDevice, texCoords.size() * sizeof(CVector2f), texCoords.data());
	static const std::map<RenderMode, VkPrimitiveTopology> topologyMap = {
		{ RenderMode::LINE_LOOP, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP },
		{ RenderMode::LINES, VK_PRIMITIVE_TOPOLOGY_LINE_LIST },
		{ RenderMode::RECTANGLES, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP },//deprecated
		{ RenderMode::TRIANGLE_STRIP, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP },
		{ RenderMode::TRIANGLES, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
	};
	m_pipelineHelper.SetTopology(topologyMap.at(mode));
	m_pipelineHelper.Bind(m_commandBuffers[m_currentCommandBufferIndex]);

	VkBuffer buffers[] = { (vertices.empty() ? *m_emptyBuffer : *m_vertexBuffer), (normals.empty() ? *m_emptyBuffer : *m_normalsBuffer), (texCoords.empty() ? *m_emptyBuffer : *m_texCoordBuffer) };
	VkDeviceSize offsets[] = { 0, 0, 0 };
	vkCmdBindVertexBuffers(m_commandBuffers[m_currentCommandBufferIndex], 0, 3, buffers, offsets);

	vkCmdDraw(m_commandBuffers[m_currentCommandBufferIndex], vertices.size(), 1, 0, 0);
}

void CVulkanRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
	std::vector<CVector3f> position;
	std::transform(vertices.begin(), vertices.end(), std::back_inserter(position), [](CVector2i const& vec) {
		return CVector3f(static_cast<float>(vec.x), static_cast<float>(vec.y), 0.0f);
	});
	RenderArrays(mode, position, {}, texCoords);
}

void CVulkanRenderer::SetColor(const float r, const float g, const float b, const float a /*= 1.0f*/)
{
	const float color[] = { r, g, b, a };
	SetColor(color);
}

void CVulkanRenderer::SetColor(const int r, const int g, const int b, const int a /*= UCHAR_MAX*/)
{
	const int color[] = { r, g, b, a };
	SetColor(color);
}

void CVulkanRenderer::SetColor(const float * color)
{
	m_shaderManager.SetUniformValue("color", 4, 1, color);
}

void CVulkanRenderer::SetColor(const int * color)
{
	auto charToFloat = [](const int value) {return static_cast<float>(value) / UCHAR_MAX; };
	float fcolor[] = { charToFloat(color[0]), charToFloat(color[1]), charToFloat(color[2]), charToFloat(color[3]) };
	SetColor(fcolor);
}

void CVulkanRenderer::PushMatrix()
{
	m_matrixManager.PushMatrix();
}

void CVulkanRenderer::PopMatrix()
{
	m_matrixManager.PopMatrix();
}

void CVulkanRenderer::Translate(const float dx, const float dy, const float dz)
{
	m_matrixManager.Translate(dx, dy, dz);
}

void CVulkanRenderer::Translate(const double dx, const double dy, const double dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void CVulkanRenderer::Translate(const int dx, const int dy, const int dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void CVulkanRenderer::Rotate(const double angle, const double x, const double y, const double z)
{
	m_matrixManager.Rotate(angle, static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void CVulkanRenderer::Scale(const double scale)
{
	m_matrixManager.Scale(static_cast<float>(scale));
}

void CVulkanRenderer::GetViewMatrix(float * matrix) const
{
	m_matrixManager.GetModelViewMatrix(matrix);
}

void CVulkanRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	m_matrixManager.LookAt(position, direction, up);
}

void CVulkanRenderer::SetTexture(std::wstring const& texture, bool forceLoadNow /*= false*/, int flags /*= 0*/)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, nullptr, flags);
}

void CVulkanRenderer::SetTexture(std::wstring const& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, flags);
}

void CVulkanRenderer::SetTexture(std::wstring const& texture, const std::vector<sTeamColor> * teamcolor, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, teamcolor, flags);
}

std::unique_ptr<ICachedTexture> CVulkanRenderer::RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height)
{
	auto texture = std::make_unique<CVulkanCachedTexture>(m_device, m_descriptorSet);
	texture->Init(width, height, m_physicalDevice);
	func();
	return std::move(texture);
}

std::unique_ptr<ICachedTexture> CVulkanRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type /*= CachedTextureType::RGBA*/)
{
	auto texture = std::make_unique<CVulkanCachedTexture>(m_device, m_descriptorSet);
	UploadTexture(*texture, (unsigned char*)data, width, height, 32, 0);
	return std::move(texture);
}

ICachedTexture* CVulkanRenderer::GetTexturePtr(std::wstring const& texture) const
{
	return m_textureManager->GetTexturePtr(texture);
}

void CVulkanRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
	static const std::string ambientKey = "material.ambient";
	static const std::string diffuseKey = "material.diffuse";
	static const std::string specularKey = "material.specular";
	static const std::string shininessKey = "material.shininess";
	m_shaderManager.SetUniformValue(ambientKey, 4, 1, ambient);
	m_shaderManager.SetUniformValue(diffuseKey, 4, 1, diffuse);
	m_shaderManager.SetUniformValue(specularKey, 4, 1, specular);
	m_shaderManager.SetUniformValue(shininessKey, 1, 1, &shininess);
}

class CMockDrawingList : public IDrawingList
{
public:
	CMockDrawingList(std::function<void() > const& func)
		:m_func(func)
	{
	}

	virtual void Draw() const override
	{
		m_func();
	}
private:
	std::function<void() > m_func;
};

std::unique_ptr<IDrawingList> CVulkanRenderer::CreateDrawingList(std::function<void() > const& func)
{
	return std::make_unique<CMockDrawingList>(func);
}

std::unique_ptr<IVertexBuffer> CVulkanRenderer::CreateVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/, size_t size /*= 0*/, bool temp /*= false*/)
{
	m_serviceCommandBuffer->WaitFence();
	auto buffer = std::make_unique<CVulkanVertexBuffer>(this, m_device, m_physicalDevice, *m_serviceCommandBuffer, vertex, normals, texcoords, size * sizeof(float));
	buffer->DoBeforeDraw(std::bind(&CMatrixManagerGLM::UpdateMatrices, &m_matrixManager, std::ref(m_shaderManager)));
	SubmitServiceCommandBuffer();
	return std::move(buffer);
}

std::unique_ptr<IOcclusionQuery> CVulkanRenderer::CreateOcclusionQuery()
{
	return std::make_unique<CVulkanOcclusionQuery>();
}

std::string CVulkanRenderer::GetName() const
{
	return "Vulkan";
}

bool CVulkanRenderer::SupportsFeature(Feature feature) const
{
	return true;
}

IShaderManager& CVulkanRenderer::GetShaderManager()
{
	return m_shaderManager;
}

void CVulkanRenderer::CreateDeviceAndQueues()
{
	uint32_t num_devices = 0;
	VkResult result = vkEnumeratePhysicalDevices(m_instance, &num_devices, nullptr);
	if (result || (num_devices == 0))
	{
		throw std::runtime_error("Cannot find any physical devices");
	}
	std::vector<VkPhysicalDevice> physical_devices(num_devices);
	result = vkEnumeratePhysicalDevices(m_instance, &num_devices, physical_devices.data());
	CHECK_VK_RESULT(result, "Failed to enumerate physical devices");
	for (uint32_t i = 0; i < num_devices; ++i)
	{
		if (CheckPhysicalDevice(physical_devices[i], m_graphicsQueueFamilyIndex))
		{
			m_physicalDevice = physical_devices[i];
			break;
		}
	}
	m_presentQueueFamilyIndex = m_graphicsQueueFamilyIndex;
	if (!m_physicalDevice)
	{
		throw std::runtime_error("Cannot find compatible physical device");
	}
	std::vector<char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	std::vector<float> queue_priorities = { 1.0f };
	VkDeviceQueueCreateInfo queue_create_info = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, m_graphicsQueueFamilyIndex, static_cast<uint32_t>(queue_priorities.size()), queue_priorities.data() };
	VkDeviceCreateInfo device_create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0, 1, &queue_create_info, 0, nullptr, static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(), nullptr };
	result = vkCreateDevice(m_physicalDevice, &device_create_info, nullptr, &m_device);
	CHECK_VK_RESULT(result, "Cannot create virtual device");
	vkGetDeviceQueue(m_device, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, m_presentQueueFamilyIndex, 0, &m_presentQueue);
}

void CVulkanRenderer::CreateSwapchain()
{
	VkBool32 supported = VK_FALSE;
	VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, m_presentQueueFamilyIndex, m_surface, &supported);
	CHECK_VK_RESULT(result, "Cannot check surface support");
	if (!supported) throw std::runtime_error("Surface is not supported");

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &surfaceCapabilities);
	CHECK_VK_RESULT(result, "Cannot get surface capabilities");

	uint32_t count;
	if ((vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &count, nullptr) != VK_SUCCESS) || (count == 0))
	{
		throw std::runtime_error("Cannot query sufrace formats");
	}
	std::vector<VkSurfaceFormatKHR> surface_formats(count);
	if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &count, surface_formats.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Cannot query sufrace formats");
	}
	if ((vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &count, nullptr) != VK_SUCCESS) || (count == 0))
	{
		throw std::runtime_error("Cannot query present modes");
	}
	std::vector<VkPresentModeKHR> present_modes(count);
	if ((vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &count, present_modes.data()) != VK_SUCCESS))
	{
		throw std::runtime_error("Cannot query present modes");
	}

	uint32_t image_count = surfaceCapabilities.minImageCount + 1;
	if ((surfaceCapabilities.maxImageCount > 0) && (image_count > surfaceCapabilities.maxImageCount)) 
	{
		image_count = surfaceCapabilities.maxImageCount;
	}
	auto surfaceFormat = GetSurfaceFormat(surface_formats);
	VkExtent2D extent = GetSurfaceExtent(surfaceCapabilities);
	VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	VkSurfaceTransformFlagBitsKHR transformFlags = (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) 
		? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR 
		: surfaceCapabilities.currentTransform;
	auto presentMode = SelectPresentMode(present_modes, { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR });

	VkSwapchainCreateInfoKHR swap_chain_create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, nullptr, 0, m_surface, image_count, surfaceFormat.format, surfaceFormat.colorSpace,
		extent, 1, imageUsageFlags, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, transformFlags, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, presentMode, VK_TRUE, m_swapchain };

	VkSwapchainKHR swapchain;
	result = vkCreateSwapchainKHR(m_device, &swap_chain_create_info, nullptr, &swapchain);
	CHECK_VK_RESULT(result, "Failed to create swapchain");
	m_swapchain.Init(swapchain, m_device, extent, surfaceFormat.format);
}

void CVulkanRenderer::CreateCommandBuffers()
{
	VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_graphicsQueueFamilyIndex };
	VkResult result = vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool);
	CHECK_VK_RESULT(result, "Cannot create command pool");

	m_commandBuffers.reserve(COMMAND_BUFFERS_COUNT);
	for (uint32_t i = 0; i < COMMAND_BUFFERS_COUNT; ++i)
	{
		m_commandBuffers.emplace_back(m_commandPool, m_device);
	}
	m_frameBuffers.resize(COMMAND_BUFFERS_COUNT);
	m_serviceCommandBuffer.reset(new CCommandBufferWrapper(m_commandPool, m_device));
}

void CVulkanRenderer::CreateRenderPass()
{
	VkAttachmentDescription attachmentDescriptions[] = { 0, m_swapchain.GetFormat(), VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR };
	VkAttachmentReference color_attachment_references[] = { { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
	VkSubpassDescription subpass_descriptions[] = { { 0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, nullptr, 1, color_attachment_references, nullptr, nullptr, 0, nullptr } };
	std::vector<VkSubpassDependency> dependencies = {
		{ VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_DEPENDENCY_BY_REGION_BIT },
		{ 0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT },
	};
	VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0, 1, attachmentDescriptions, 1, subpass_descriptions, static_cast<uint32_t>(dependencies.size()), dependencies.data() };
	VkResult result = vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &m_renderPass);
	CHECK_VK_RESULT(result, "cannot create render pass");
}

void CVulkanRenderer::InitFramebuffer()
{
	m_frameBuffers[m_currentCommandBufferIndex].Destroy();
	VkImageView view = m_swapchain.GetImageView(m_currentImageIndex);
	VkExtent2D size = m_swapchain.GetExtent();
	VkFramebufferCreateInfo framebuffer_create_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, nullptr, 0, m_renderPass, 1, &view, size.width, size.height, 1 };
	VkResult result = vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_frameBuffers[m_currentCommandBufferIndex]);
	LOG_VK_RESULT(result, "Failed to create framebuffer");
	m_frameBuffers[m_currentCommandBufferIndex].SetDevice(m_device);
}

void CVulkanRenderer::CreateDescriptors()
{
	VkDescriptorSetLayoutBinding layout_bindings[] = {
		{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr },
		{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
		{ 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
	};
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, nullptr, 0, sizeof(layout_bindings) / sizeof(layout_bindings[0]), layout_bindings };
	VkResult result = vkCreateDescriptorSetLayout(m_device, &descriptor_set_layout_create_info, nullptr, &m_descriptorSetLayout);
	CHECK_VK_RESULT(result, "cannot create descriptor set layout");
	m_descriptorSetLayout.SetDevice(m_device);

	VkDescriptorPoolSize pool_sizes[] = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
	};
	VkDescriptorPoolCreateInfo descriptor_pool_create_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr, 0, 1, sizeof(pool_sizes) / sizeof(pool_sizes[0]), pool_sizes };
	result = vkCreateDescriptorPool(m_device, &descriptor_pool_create_info, nullptr, &m_desciptorPool);
	CHECK_VK_RESULT(result, "cannot create descriptor pool");
	m_desciptorPool.SetDevice(m_device);

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, m_desciptorPool, 1, &m_descriptorSetLayout };
	result = vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, &m_descriptorSet);
	CHECK_VK_RESULT(result, "cannot create descriptor pool");
}

void CVulkanRenderer::SubmitServiceCommandBuffer()
{
	VkCommandBuffer buffer = *m_serviceCommandBuffer;
	VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 0, nullptr,nullptr, 1, &buffer, 0, nullptr };
	VkResult result = vkQueueSubmit(m_graphicsQueue, 1, &submit_info, m_serviceCommandBuffer->GetFence());
	LOG_VK_RESULT(result, "Cannot submit service command buffer to queue");
}

void CPipelineHelper::Init(VkDevice device, VkRenderPass pass)
{
	m_pipelineLayout.Destroy();
	VkResult result = vkCreatePipelineLayout(device, &layout_create_info, nullptr, &m_pipelineLayout);
	m_pipelineLayout.SetDevice(device);
	pipeline_create_info.layout = m_pipelineLayout;
	CHECK_VK_RESULT(result, "Cannot create pipeline layout");
	pipeline_create_info.renderPass = pass;
	m_device = device;
}


VkPipeline CPipelineHelper::GetPipeline()
{
	auto it = m_pipelines.find(m_currentKey);
	if (it == m_pipelines.end())
	{
		VkPipeline pipeline;
		VkResult result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline);
		CHECK_VK_RESULT(result, "Cannot create pipeline");
		it = m_pipelines.emplace(std::make_pair(m_currentKey, pipeline)).first;
	}
	return it->second;
}


void CPipelineHelper::Bind(VkCommandBuffer commandBuffer)
{
	VkPipeline newPipeline = GetPipeline();
	if (newPipeline != m_currentLayout)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, newPipeline);
	}
}

void CPipelineHelper::SetShaderProgram(CVulkanShaderProgram const& program)
{
	pipeline_create_info.stageCount = program.GetShaderInfo().size();
	pipeline_create_info.pStages = program.GetShaderInfo().data();
	m_currentKey.program = &program;
}

void CPipelineHelper::SetVertexAttributes(std::vector<VertexAttrib> const& attribs)
{
	vertex_binding_descriptions.clear();
	vertex_attribute_descriptions.clear();
	for (auto& attrib : attribs)
	{
		vertex_binding_descriptions.push_back({ vertex_binding_descriptions.size(), attrib.size, attrib.perInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX });
		vertex_attribute_descriptions.push_back({ attrib.pos, vertex_binding_descriptions.back().binding, attrib.format, 0 });
	}
	vertex_input_state_create_info.vertexBindingDescriptionCount = vertex_binding_descriptions.size();
	vertex_input_state_create_info.pVertexBindingDescriptions = vertex_binding_descriptions.data();
	vertex_input_state_create_info.vertexAttributeDescriptionCount = vertex_attribute_descriptions.size();
	vertex_input_state_create_info.pVertexAttributeDescriptions = vertex_attribute_descriptions.data();
	m_currentKey.attribs = attribs;
}

void CPipelineHelper::SetDescriptorLayout(VkDescriptorSetLayout * layouts, uint32_t count)
{
	layout_create_info.setLayoutCount = count;
	layout_create_info.pSetLayouts = layouts;
	m_currentKey.descriptors = layouts[0];
}


void CPipelineHelper::SetTopology(VkPrimitiveTopology topology)
{
	input_assembly_state_create_info.topology = topology;
	m_currentKey.topology = topology;
}

void CPipelineHelper::Destroy()
{
	for (auto& pair : m_pipelines)
	{
		vkDestroyPipeline(m_device, pair.second, nullptr);
	}
	m_pipelines.clear();
	m_pipelineLayout.Destroy();
}

CCommandBufferWrapper::CCommandBufferWrapper(VkCommandPool pool, VkDevice device)
	: m_device(device), m_pool(pool)
{
	m_imageAvailibleSemaphore.SetDevice(device);
	m_renderingFinishedSemaphore.SetDevice(device);

	VkSemaphoreCreateInfo semaphore_create_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0 };
	if (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &m_imageAvailibleSemaphore) || vkCreateSemaphore(device, &semaphore_create_info, nullptr, &m_renderingFinishedSemaphore))
	{
		throw std::runtime_error("Cannot create semaphores");
	}

	VkFenceCreateInfo fence_create_info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT };
	VkResult result = vkCreateFence(m_device, &fence_create_info, nullptr, &m_fence);
	CHECK_VK_RESULT(result, "Cannot create fence");
	m_fence.SetDevice(device);

	VkCommandBufferAllocateInfo allocateBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr, pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1 };
	result = vkAllocateCommandBuffers(m_device, &allocateBufferInfo, &m_commandBuffer);
	CHECK_VK_RESULT(result, "Cannot create command buffer");
}

CCommandBufferWrapper::~CCommandBufferWrapper()
{
	if (m_device && m_commandBuffer && m_pool)
	{
		vkFreeCommandBuffers(m_device, m_pool, 1, &m_commandBuffer);
	}
}

void CCommandBufferWrapper::WaitFence()
{
	VkResult result = vkWaitForFences(m_device, 1, &m_fence, VK_FALSE, 1000000000);
	LOG_VK_RESULT(result, "Waiting on fence takes too long");
	vkResetFences(m_device, 1, &m_fence);
}

void CSwapchainWrapper::Init(VkSwapchainKHR swapchain, VkDevice device, VkExtent2D extent, VkFormat format)
{
	m_swapchain.Destroy();
	m_imageViews.clear();
	m_images.clear();
	*&m_swapchain = swapchain;
	m_swapchain.SetDevice(device);
	uint32_t count;
	VkResult result = vkGetSwapchainImagesKHR(device, m_swapchain, &count, nullptr);
	CHECK_VK_RESULT(result, "Cannot get swapchain images");
	m_images.resize(count);
	result = vkGetSwapchainImagesKHR(device, m_swapchain, &count, m_images.data());
	CHECK_VK_RESULT(result, "Cannot get swapchain images");

	m_imageViews.resize(count);
	VkImageViewCreateInfo image_view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0, VK_NULL_HANDLE, VK_IMAGE_VIEW_TYPE_2D, format,
	{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } };
	for (uint32_t i = 0; i < count; ++i)
	{
		image_view_create_info.image = m_images[i];
		result = vkCreateImageView(device, &image_view_create_info, nullptr, &m_imageViews[i]);
		CHECK_VK_RESULT(result, "Failed to create image view for framebuffer");
		m_imageViews[i].SetDevice(device);
	}
	m_extent = extent;
	m_format = format;
}

CVulkanCachedTexture::CVulkanCachedTexture(VkDevice device, VkDescriptorSet descriptorSet)
	: m_device(device), m_descriptorSet(descriptorSet)
{
}

void CVulkanCachedTexture::Init(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice)
{
	VkImageCreateInfo image_create_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,  nullptr, 0, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM,{ width, height, 1 }, 1, 1, VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_IMAGE_LAYOUT_PREINITIALIZED };
	VkResult result = vkCreateImage(m_device, &image_create_info, nullptr, &m_image);
	m_image.SetDevice(m_device);

	VkMemoryRequirements image_memory_requirements;
	vkGetImageMemoryRequirements(m_device, m_image, &image_memory_requirements);
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory_properties);
	uint32_t property = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
	{
		if ((image_memory_requirements.memoryTypeBits & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & property))
		{
			VkMemoryAllocateInfo memory_allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, image_memory_requirements.size, i };
			if (vkAllocateMemory(m_device, &memory_allocate_info, nullptr, &m_memory) == VK_SUCCESS)
			{
				break;
			}
		}
	}
	if (!m_memory)
	{
		LogWriter::WriteLine("Cannot allocate memory for image");
	}
	m_memory.SetDevice(m_device);
	result = vkBindImageMemory(m_device, m_image, m_memory, 0);
	LOG_VK_RESULT(result, "Cannot bind memory to an image");

	VkImageViewCreateInfo image_view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0, m_image, VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM,
	{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } };
	result = vkCreateImageView(m_device, &image_view_create_info, nullptr, &m_imageView);
	LOG_VK_RESULT(result, "Cannot create imageView");
	m_imageView.SetDevice(m_device);

	VkSamplerCreateInfo sampler_create_info = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, nullptr, 0, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.0f, VK_FALSE, 1.0f, VK_FALSE, VK_COMPARE_OP_ALWAYS, 0.0f,  0.0f, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, VK_FALSE };
	result = vkCreateSampler(m_device, &sampler_create_info, nullptr, &m_sampler);
	LOG_VK_RESULT(result, "Cannot create sampler");
	m_sampler.SetDevice(m_device);

	m_size = image_memory_requirements.size;
	m_extent = { width, height, 1 };
}

void CVulkanCachedTexture::Upload(const void * data, VkCommandBuffer commandBuffer)
{
	void *staging_buffer_memory_pointer;
	VkResult result = vkMapMemory(m_device, m_memory, 0, m_size, 0, &staging_buffer_memory_pointer);
	LOG_VK_RESULT(result, "Cannot map memory for image");

	VkImageSubresource subresource = {};
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel = 0;
	subresource.arrayLayer = 0;

	VkSubresourceLayout stagingImageLayout;
	vkGetImageSubresourceLayout(m_device, m_image, &subresource, &stagingImageLayout);
	if (stagingImageLayout.rowPitch == m_extent.width * 4)
	{
		memcpy(staging_buffer_memory_pointer, data, static_cast<size_t>(m_size));
	}
	else
	{
		const uint8_t* dstBytes = reinterpret_cast<const uint8_t*>(staging_buffer_memory_pointer);
		const uint8_t* srcBytes = reinterpret_cast<const uint8_t*>(data);

		for (uint32_t y = 0; y < m_extent.height; y++) 
		{
			memcpy((void*)&dstBytes[y * stagingImageLayout.rowPitch], (void*)&srcBytes[y * m_extent.width * 4], m_extent.width * 4);
		}
	}
	vkUnmapMemory(m_device, m_memory);
}

void CVulkanCachedTexture::Bind() const
{
	if (!m_image) return;
	VkDescriptorImageInfo image_info = { m_sampler, m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
	VkWriteDescriptorSet descriptor_writes = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, m_descriptorSet, 2, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &image_info, nullptr, nullptr };
	vkUpdateDescriptorSets(m_device, 1, &descriptor_writes, 0, nullptr);
}

void CVulkanCachedTexture::UnBind() const
{
}

CVulkanVertexBuffer::CVulkanVertexBuffer(CVulkanRenderer * renderer, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer, const float * vertex, const float * normals, const float * texcoords, size_t size)
	: m_vertexCache(size * 3 * sizeof(float), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, device, physicalDevice)
	, m_normalsCache(normals ? size * 3 * sizeof(float) : 0, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, device, physicalDevice)
	, m_texcoordCache(texcoords ? size * 2 * sizeof(float) : 0, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, device, physicalDevice)
	, m_renderer(renderer)
{
	VkCommandBufferBeginInfo command_buffer_begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr };
	vkBeginCommandBuffer(commandBuffer, &command_buffer_begin_info);
	m_vertexCache.Upload(vertex, size * 3 * sizeof(float), commandBuffer);
	if(normals) m_normalsCache.Upload(normals, size * 3 * sizeof(float), commandBuffer);
	if(texcoords) m_texcoordCache.Upload(texcoords, size * 2 * sizeof(float), commandBuffer);
	vkEndCommandBuffer(commandBuffer);
}

void CVulkanVertexBuffer::SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize)
{
	m_indexCache = std::make_unique<CStagedVulkanVertexAttribCache>(indexesSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_renderer->GetDevice(), m_renderer->GetPhysicalDevice());
	VkCommandBufferBeginInfo command_buffer_begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr };
	VkCommandBuffer commandBuffer = m_renderer->GetServiceCommandBuffer();
	vkBeginCommandBuffer(commandBuffer, &command_buffer_begin_info);
	m_indexCache->Upload(indexPtr, indexesSize * sizeof(float), commandBuffer);
	vkEndCommandBuffer(commandBuffer);
	m_renderer->SubmitServiceCommandBuffer();
}

void CVulkanVertexBuffer::Bind() const
{
	std::vector<CPipelineHelper::VertexAttrib> attribs;
	auto& pipelineHelper = m_renderer->GetPipelineHelper();
	pipelineHelper.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineHelper.Bind(m_renderer->GetCommandBuffer());

	VkBuffer buffers[] = { m_vertexCache.GetSize() > 0 ? m_vertexCache : m_renderer->GetEmptyBuffer(), m_normalsCache.GetSize() > 0 ? m_normalsCache : m_renderer->GetEmptyBuffer(), m_texcoordCache.GetSize() > 0 ? m_texcoordCache : m_renderer->GetEmptyBuffer() };
	VkDeviceSize offsets[] = { 0, 0, 0 };
	vkCmdBindVertexBuffers(m_renderer->GetCommandBuffer(), 0, 3, buffers, offsets);
	if (m_indexCache)
	{
		vkCmdBindIndexBuffer(m_renderer->GetCommandBuffer(), *m_indexCache, 0, VK_INDEX_TYPE_UINT32);
	}
}

void CVulkanVertexBuffer::DrawIndexes(size_t begin, size_t count)
{
	if (m_beforeDraw) m_beforeDraw();
	vkCmdDrawIndexed(m_renderer->GetCommandBuffer(), count, 1, begin, 0, 0);
}

void CVulkanVertexBuffer::DrawAll(size_t count)
{
	if (m_beforeDraw) m_beforeDraw();
	vkCmdDraw(m_renderer->GetCommandBuffer(), count, 1, 0, 0);
}

void CVulkanVertexBuffer::DrawInstanced(size_t size, size_t instanceCount)
{
	if (m_beforeDraw) m_beforeDraw();
	vkCmdDraw(m_renderer->GetCommandBuffer(), size, instanceCount, 0, 0);
}

void CVulkanVertexBuffer::UnBind() const
{
}

void CVulkanVertexBuffer::DoBeforeDraw(std::function<void()> const& handler)
{
	m_beforeDraw = handler;
}
