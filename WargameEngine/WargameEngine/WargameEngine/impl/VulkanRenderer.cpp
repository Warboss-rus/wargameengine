#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)
namespace glm
{
template <typename T>
GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> perspectiveFovLHZeroToOne(T fov, T width, T height, T zNear, T zFar)
{
	assert(width > static_cast<T>(0));
	assert(height > static_cast<T>(0));
	assert(fov > static_cast<T>(0));

	T const rad = fov;
	T const h = glm::cos(static_cast<T>(0.5) * rad) / glm::sin(static_cast<T>(0.5) * rad);
	T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

	tmat4x4<T, defaultp> Result(static_cast<T>(0));
	Result[0][0] = w;
	Result[1][1] = h;
	Result[2][3] = static_cast<T>(1);

#if GLM_DEPTH_CLIP_SPACE == GLM_DEPTH_ZERO_TO_ONE
	Result[2][2] = zFar / (zFar - zNear);
	Result[3][2] = -(zFar * zNear) / (zFar - zNear);
#		else
	Result[2][2] = (zFar + zNear) / (zFar - zNear);
	Result[3][2] = -(static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
#		endif

	return Result;
}

template <typename T, typename U, precision P>
GLM_FUNC_QUALIFIER tvec3<T, P> unProjectZeroToOne(tvec3<T, P> const & win, tmat4x4<T, P> const & model, tmat4x4<T, P> const & proj, tvec4<U, P> const & viewport)
{
	tmat4x4<T, P> Inverse = inverse(proj * model);

	tvec4<T, P> tmp = tvec4<T, P>(win, T(1));
	tmp.x = (tmp.x - T(viewport[0])) / T(viewport[2]);
	tmp.y = (tmp.y - T(viewport[1])) / T(viewport[3]);
#		if GLM_DEPTH_CLIP_SPACE == GLM_DEPTH_ZERO_TO_ONE
	tmp.x = tmp.x * static_cast<T>(2) - static_cast<T>(1);
	tmp.y = tmp.y * static_cast<T>(2) - static_cast<T>(1);
#		else
	tmp = tmp * static_cast<T>(2) - static_cast<T>(1);
#		endif

	tvec4<T, P> obj = Inverse * tmp;
	obj /= obj.w;

	return tvec3<T, P>(obj);
}
template <typename T, typename U, precision P>
GLM_FUNC_QUALIFIER tvec3<T, P> projectZeroToOne(tvec3<T, P> const & obj, tmat4x4<T, P> const & model, tmat4x4<T, P> const & proj, tvec4<U, P> const & viewport)
{
	tvec4<T, P> tmp = tvec4<T, P>(obj, static_cast<T>(1));
	tmp = model * tmp;
	tmp = proj * tmp;

	tmp /= tmp.w;
#		if GLM_DEPTH_CLIP_SPACE == GLM_DEPTH_ZERO_TO_ONE
	tmp.x = tmp.x * static_cast<T>(0.5) + static_cast<T>(0.5);
	tmp.y = tmp.y * static_cast<T>(0.5) + static_cast<T>(0.5);
#		else
	tmp = tmp * static_cast<T>(0.5) + static_cast<T>(0.5);
#		endif
	tmp[0] = tmp[0] * T(viewport[2]) + T(viewport[0]);
	tmp[1] = tmp[1] * T(viewport[3]) + T(viewport[1]);

	return tvec3<T, P>(tmp);
}
}
#include "VulkanRenderer.h"
#include "../LogWriter.h"
#include <iterator>
#include <algorithm>
#include "../view/TextureManager.h"
#include "../view/IViewport.h"
#include "../Utils.h"

namespace
{
constexpr uint32_t COMMAND_BUFFERS_COUNT = 3;
constexpr int BUFFER_DELAY_FRAMES = COMMAND_BUFFERS_COUNT + 1;
constexpr int RESOURCE_DELAY_FRAMES = COMMAND_BUFFERS_COUNT * 20;
#ifndef clamp
#define clamp(value, minV, maxV) value = std::min(std::max(value, minV), maxV)
#endif

#ifdef _DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
{
	(void)flags; (void)objType; (void)obj; (void)location; (void)code; (void)layerPrefix; (void)userData;
	LogWriter::WriteLine(msg);
	return VK_FALSE;
}
#endif

bool CheckPhysicalDevice(VkPhysicalDevice device, uint32_t & queue_family_index)
{
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);
	const uint32_t major_version = VK_VERSION_MAJOR(properties.apiVersion);
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
	for (const VkPresentModeKHR desiredMode : desiredPresentModes)
	{
		if (std::find(supportedPresentModes.begin(), supportedPresentModes.end(), desiredMode) != supportedPresentModes.end())
		{
			return desiredMode;
		}
	}
	return supportedPresentModes.front();
}

VkFormat GetTextureFormat(CachedTextureType type, int flags)
{
	switch (type)
	{
	case CachedTextureType::ALPHA:
		return VK_FORMAT_R8_UNORM;
	case CachedTextureType::DEPTH:
		return VK_FORMAT_D32_SFLOAT;
	case CachedTextureType::RENDER_TARGET:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case CachedTextureType::RGBA:
		return (flags & TEXTURE_HAS_ALPHA)
			? (flags & TEXTURE_BGRA ? VK_FORMAT_B8G8R8A8_UNORM : VK_FORMAT_R8G8B8A8_UNORM)
			: (flags & TEXTURE_BGRA ? VK_FORMAT_B8G8R8_UNORM : VK_FORMAT_R8G8B8_UNORM);
	default:
		return VK_FORMAT_R8G8B8A8_UNORM;
	}
}

class CVulkanVertexBuffer : public IVertexBuffer
{
public:
	CVulkanVertexBuffer(CVulkanRenderer * renderer, VkCommandBuffer commandBuffer, const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0);
	virtual void SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize) override;
	virtual void Bind() const override;
	virtual void DrawIndexes(size_t begin, size_t count) override;
	virtual void DrawAll(size_t count) override;
	virtual void DrawInstanced(size_t size, size_t instanceCount) override;
	virtual void UnBind() const override;
private:
	VkDeviceSize m_size;
	CVulkanVertexAttribCache m_vertexCache;
	std::unique_ptr<CVulkanVertexAttribCache> m_indexCache;
	CVulkanRenderer * m_renderer;
	VkDeviceSize m_offsets[3];
};

class CTempVulkanVertexBuffer : public IVertexBuffer
{
public:
	CTempVulkanVertexBuffer(CVulkanRenderer * renderer, const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0)
		: m_renderer(renderer), m_buffers{ renderer->GetEmptyBuffer(), renderer->GetEmptyBuffer(), renderer->GetEmptyBuffer() }
	{
		void* data;
		auto& smartBuffer = renderer->GetVertexBuffer();
		if (vertex)
		{
			std::tie(m_buffers[0], m_offsets[0], data) = smartBuffer.Allocate(size * 3 * sizeof(float));
			memcpy(data, vertex, size * 3 * sizeof(float));
		}
		if (normals)
		{
			std::tie(m_buffers[1], m_offsets[1], data) = smartBuffer.Allocate(size * 3 * sizeof(float));
			memcpy(data, normals, size * 3 * sizeof(float));
		}
		if (texcoords)
		{
			std::tie(m_buffers[2], m_offsets[2], data) = smartBuffer.Allocate(size * 2 * sizeof(float));
			memcpy(data, texcoords, size * 2 * sizeof(float));
		}
		
	}
	void SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize) override
	{
		throw std::runtime_error("no support for temp indexes yet");
	}
	void Bind() const override
	{
		auto& pipelineHelper = m_renderer->GetPipelineHelper();
		pipelineHelper.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		pipelineHelper.Bind(m_renderer->GetCommandBuffer());
		vkCmdBindVertexBuffers(m_renderer->GetCommandBuffer(), 0, 3, m_buffers, m_offsets);
		if (m_indexBuffer)
		{
			vkCmdBindIndexBuffer(m_renderer->GetCommandBuffer(), m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}
	void DrawIndexes(size_t begin, size_t count) override
	{
		if (count == 0) return;
		m_renderer->BeforeDraw();
		vkCmdDrawIndexed(m_renderer->GetCommandBuffer(), count, 1, begin, 0, 0);
	}

	void DrawAll(size_t count) override
	{
		if (count == 0) return;
		m_renderer->BeforeDraw();
		vkCmdDraw(m_renderer->GetCommandBuffer(), count, 1, 0, 0);
	}

	void DrawInstanced(size_t size, size_t instanceCount) override
	{
		if (size == 0 || instanceCount == 0) return;
		m_renderer->BeforeDraw();
		vkCmdDraw(m_renderer->GetCommandBuffer(), size, instanceCount, 0, 0);
	}

	void UnBind() const override {}
private:
	CVulkanRenderer * m_renderer;
	VkBuffer m_buffers[3];
	VkDeviceSize m_offsets[3] = { 0, 0, 0 };
	VkBuffer m_indexBuffer = VK_NULL_HANDLE;
	VkDeviceSize m_indexOffset = 0;
};
}

CVulkanRenderer::CVulkanRenderer(const std::vector<const char*> & instanceExtensions)
	:m_shaderManager(*this)
{
	const VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "WargameEngine", VK_MAKE_VERSION(1, 0, 0), "WargameEngine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0 };
	const std::vector<const char*> validationLayers = {
#ifdef _DEBUG
		"VK_LAYER_LUNARG_standard_validation"
#endif
	};

	const VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &appInfo, static_cast<uint32_t>(validationLayers.size()), validationLayers.data(), static_cast<uint32_t>(instanceExtensions.size()), instanceExtensions.data() };
	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_instance);
	CHECK_VK_RESULT(result, "Cannot create vulkan instance");
#ifdef _DEBUG
	const VkDebugReportCallbackCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT, nullptr, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, DebugCallback, this };
	const auto CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
	result = CreateDebugReportCallback(m_instance, &createInfo, nullptr, &m_debugCallback);
	m_debugCallbackDestructor.SetDestructorFunction([this] {
		const auto DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");
		DestroyDebugReportCallback(m_instance, m_debugCallback, nullptr);
	});
#endif

	CreateDeviceAndQueues();
	
	m_commandPool.SetDevice(m_device);
	m_renderPass.SetDevice(m_device);

	m_memoryManager = std::make_unique<CVulkanMemoryManager>(8 * 1024 * 1024, m_device, m_physicalDevice);//8MB pieces
	m_memoryManager->SetResourceFreeDelay(RESOURCE_DELAY_FRAMES + 1);

	m_descriptorSetManager.Init(m_device, 10000);

	m_emptyTexture = std::make_unique<CVulkanCachedTexture>(*this);
	m_emptyTexture->Init(1, 1, *m_memoryManager, CachedTextureType::RGBA, TEXTURE_HAS_ALPHA);

	std::array<float, 1000> zero;
	zero.fill(0.0f);
	m_emptyBuffer = std::make_unique<CVulkanVertexAttribCache>(sizeof(float) * zero.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, *this, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, zero.data());

	m_shaderManager.DoOnProgramChange([this](const CVulkanShaderProgram & program) {
		program.Commit(m_activeCommandBuffer ? m_activeCommandBuffer->GetUniformBuffer() : m_serviceCommandBuffer->GetUniformBuffer(), true);
		m_descriptorSetManager.SetShaderProgram(&program, (m_activeCommandBuffer ? *m_activeCommandBuffer : (VkCommandBuffer)VK_NULL_HANDLE), m_pipelineHelper.GetLayout());
	});
}

CVulkanRenderer::~CVulkanRenderer()
{
	m_swapchain.DestroyDepthTexture();
	if (m_device)
	{
		vkDeviceWaitIdle(m_device);
	}
	FreeResources(true);
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
	m_renderPass = CreateRenderPass(m_swapchain.GetFormat(), m_swapchain.GetDepthTexture().GetFormat());
	m_renderPass.SetDevice(m_device);
#ifdef TO_STRING_HACK
	const Path shaderLocation = make_path("/sdcard/WargameEngine/Killteam/shaders/Vulkan/");
#else
	const Path shaderLocation = make_path(L"Killteam/shaders/Vulkan/");
#endif
	m_defaultProgram = m_shaderManager.NewProgram(shaderLocation + make_path("vert.spv"), shaderLocation + make_path("frag.spv"));
	m_shaderManager.PushProgram(*m_defaultProgram);
	m_pipelineHelper.SetShaderProgram(*reinterpret_cast<CVulkanShaderProgram*>(m_defaultProgram.get()));
	m_pipelineHelper.SetVertexAttributes({
		{ 0, sizeof(float) * 3, VK_FORMAT_R32G32B32_SFLOAT, false },
		{ 1, sizeof(float) * 3, VK_FORMAT_R32G32B32_SFLOAT, false },
		{ 2, sizeof(float) * 2, VK_FORMAT_R32G32_SFLOAT, false },
	});
	const VkDescriptorSetLayout layouts[] = { m_descriptorSetManager.GetProgramLayout(), m_descriptorSetManager.GetTextureLayout() };
	m_pipelineHelper.SetDescriptorLayout(layouts, 2);
	m_pipelineHelper.Init(m_device);
	m_pipelineHelper.SetRenderPass(m_renderPass);

	BeginServiceCommandBuffer();
	std::array<unsigned char, 4> emptyTextureData = { 0, 0, 0, 255 };
	m_emptyTexture->Upload(emptyTextureData.data(), *m_memoryManager, *m_serviceCommandBuffer);

	UnbindTexture();
}

void CVulkanRenderer::Resize()
{
	CreateSwapchain();
}

void CVulkanRenderer::AcquireImage()
{
	m_currentCommandBufferIndex = (m_currentCommandBufferIndex + 1) % COMMAND_BUFFERS_COUNT;
	m_activeCommandBuffer = &m_commandBuffers[m_currentCommandBufferIndex];
	m_activeCommandBuffer->WaitFence();
	FreeResources(false);
	
	const VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_activeCommandBuffer->GetImageAvailibleSemaphore(), VK_NULL_HANDLE, &m_currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return CreateSwapchain();
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		LogWriter::WriteLine("Cannot acquire image");
	}

	m_currentImage = m_swapchain.GetImages()[m_currentImageIndex];
	InitFramebuffer(true);
	m_activeCommandBuffer->Begin();

	const VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	const VkImageMemoryBarrier barrierFromPresentToDraw = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, 0, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_GENERAL, m_presentQueueFamilyIndex, m_graphicsQueueFamilyIndex, m_currentImage, imageSubresourceRange };
	vkCmdPipelineBarrier(*m_activeCommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromPresentToDraw);
	m_descriptorSetManager.BindAll(*m_activeCommandBuffer, m_pipelineHelper.GetLayout());
}

void CVulkanRenderer::Present()
{
	vkCmdEndRenderPass(*m_activeCommandBuffer);

	if (m_graphicsQueue != m_presentQueue)
	{
		const VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		const VkImageMemoryBarrier barrierFromDrawToPresent = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, m_graphicsQueueFamilyIndex, m_presentQueueFamilyIndex, m_currentImage, imageSubresourceRange };
		vkCmdPipelineBarrier(*m_activeCommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierFromDrawToPresent);
	}
	const VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	if (m_serviceBufferIsActive)
	{
		m_serviceCommandBuffer->End();

		const VkCommandBuffer bufferHandle = *m_serviceCommandBuffer;
		const VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 0, nullptr, &mask, 1, &bufferHandle, 0, nullptr };
		const VkResult result = vkQueueSubmit(m_presentQueue, 1, &submit_info, m_serviceCommandBuffer->GetFence());
		LOG_VK_RESULT(result, "Cannot submit service buffer");

		m_serviceCommandBuffer->WaitFence();
		m_serviceBufferIsActive = false;
	}

	m_activeCommandBuffer->End();

	const VkCommandBuffer bufferHandle = *m_activeCommandBuffer;
	const VkSemaphore imageAvailibleSemaphore = m_activeCommandBuffer->GetImageAvailibleSemaphore();
	const VkSemaphore renderingFinishedSemaphore = m_activeCommandBuffer->GetRenderingFinishedSemaphore();
	const VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 1, &imageAvailibleSemaphore, &mask, 1, &bufferHandle, 1, &renderingFinishedSemaphore };
	VkResult result = vkQueueSubmit(m_presentQueue, 1, &submit_info, m_activeCommandBuffer->GetFence());
	LOG_VK_RESULT(result, "Cannot submit buffer");

	const VkSwapchainKHR swapchain = m_swapchain;
	const VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, nullptr, 1, &renderingFinishedSemaphore, 1, &swapchain, &m_currentImageIndex, nullptr };
	result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return CreateSwapchain();
	}
	LOG_VK_RESULT(result, L"Present failed");
	m_activeCommandBuffer = nullptr;
}


void CVulkanRenderer::BeforeDraw()
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	m_shaderManager.CommitUniforms(m_activeCommandBuffer->GetUniformBuffer(), false);
	m_descriptorSetManager.SetShaderProgram(m_shaderManager.GetActiveProgram(), *m_activeCommandBuffer, m_pipelineHelper.GetLayout());
}

void CVulkanRenderer::DestroyImage(CVulkanCachedTexture * texture, VkImage image, VkImageView view)
{
	if (texture)
	{
		if (*texture) m_imagesToDestroy.push_back(std::make_pair((VkImage)*texture, RESOURCE_DELAY_FRAMES));
		if (texture->GetImageView()) m_imageViewsToDestroy.push_back(std::make_pair(texture->GetImageView(), RESOURCE_DELAY_FRAMES));
		if (texture->GetSampler()) m_samplersToDestroy.push_back(std::make_pair(texture->GetSampler(), RESOURCE_DELAY_FRAMES));
		const VkDescriptorSet set = m_descriptorSetManager.GetTextureDescriptor(texture);
		if (set) m_descriptorsToDestroy.push_back(std::make_pair(set, RESOURCE_DELAY_FRAMES));
	}
	if (image) m_imagesToDestroy.push_back(std::make_pair(image, RESOURCE_DELAY_FRAMES));
	if(view) m_imageViewsToDestroy.push_back(std::make_pair(view, RESOURCE_DELAY_FRAMES));
}

void CVulkanRenderer::DestroyBuffer(VkBuffer buffer)
{
	if (buffer)
	{
		m_buffersToDestroy.push_back(std::make_pair(buffer, BUFFER_DELAY_FRAMES));
	}
}

void CVulkanRenderer::EnableMultisampling(bool enable)
{
}

void CVulkanRenderer::WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const
{
	glm::vec4 viewportData(viewport.GetX(), viewport.GetY(), viewport.GetWidth(), viewport.GetHeight());
	//Set OpenGL Windows coordinates
	float winX = (float)x;
	float winY = (float)y;

	auto ToVector3f = [](glm::vec3 const& v)->CVector3f { return{ v.x, v.y, v.z }; };
	float projectionMatrix[16];
	float viewMatrix[16];
	m_matrixManager.GetProjectionMatrix(projectionMatrix);
	m_matrixManager.GetViewMatrix(viewMatrix);
	//Cast a ray from eye to mouse cursor;
	glm::mat4 proj = glm::make_mat4(projectionMatrix);
	glm::mat4 view = glm::make_mat4(viewMatrix);
	start = ToVector3f(glm::unProjectZeroToOne(glm::vec3(winX, winY, 0.0f), view, proj, viewportData));
	end = ToVector3f(glm::unProjectZeroToOne(glm::vec3(winX, winY, 1.0f), view, proj, viewportData));
}

void CVulkanRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	glm::vec4 viewportData(viewport.GetX(), viewport.GetY(), viewport.GetWidth(), viewport.GetHeight());
	float projectionMatrix[16];
	float viewMatrix[16];
	m_matrixManager.GetProjectionMatrix(projectionMatrix);
	m_matrixManager.GetViewMatrix(viewMatrix);
	auto windowPos = glm::projectZeroToOne(glm::make_vec3(worldCoords.ptr()), glm::make_mat4(viewMatrix), glm::make_mat4(projectionMatrix), viewportData);
	x = static_cast<int>(windowPos.x);
	y = static_cast<int>(viewportData[3] - windowPos.y);
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
	//m_pipelineHelper.SetDepthParams(enable, enable);
}

void CVulkanRenderer::EnableBlending(bool enable)
{
	m_pipelineHelper.SetBlending(enable);
}

void CVulkanRenderer::SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane /*= 1.0f*/, float farPane /*= 1000.0f*/)
{
	m_viewport = { static_cast<float>(viewportX), static_cast<float>(viewportY), static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), nearPane, farPane };
	const VkRect2D scissor = { {static_cast<int32_t>(viewportX), static_cast<int32_t>(viewportY)}, {viewportWidth, viewportHeight} };
	vkCmdSetViewport(*m_activeCommandBuffer, 0, 1, &m_viewport);
	vkCmdSetScissor(*m_activeCommandBuffer, 0, 1, &scissor);
	glm::mat4 mat = glm::perspectiveFovLHZeroToOne(glm::radians(viewingAngle), static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), nearPane, farPane);
	m_matrixManager.SetProjectionMatrix(glm::value_ptr(mat));
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

void CVulkanRenderer::ClearBuffers(bool /*color = true*/, bool depth /*= true*/)
{
	VkClearValue clearValues[2];
	clearValues[0].color = VkClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = VkClearDepthStencilValue{ 1.0f, 0u };

	const VkRenderPassBeginInfo render_pass_begin_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, nullptr, m_renderPass, m_activeCommandBuffer->GetFrameBuffer() ,{ { 0, 0 }, m_swapchain.GetExtent() }, depth ? 2u : 1u, clearValues };
	vkCmdBeginRenderPass(*m_activeCommandBuffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(*m_activeCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineHelper.GetPipeline());
}

void CVulkanRenderer::SetTextureManager(CTextureManager & textureManager)
{
	m_textureManager = &textureManager;
}

void CVulkanRenderer::UnbindTexture(TextureSlot slot /*= TextureSlot::eDiffuse*/)
{
	SetTexture(*m_emptyTexture);
}

std::unique_ptr<ICachedTexture> CVulkanRenderer::CreateEmptyTexture(bool cubemap /*= false*/)
{
	return std::make_unique<CVulkanCachedTexture>(*this);
}

void CVulkanRenderer::SetTextureAnisotropy(float value /*= 1.0f*/)
{
}

void CVulkanRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
	auto& vulkanTexture = reinterpret_cast<CVulkanCachedTexture&>(texture);
	vulkanTexture.Init(width, height, *m_memoryManager, CachedTextureType::RGBA, flags);
	BeginServiceCommandBuffer();
	vulkanTexture.Upload(data, *m_memoryManager, *m_serviceCommandBuffer);
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
	return false;
}

void CVulkanRenderer::RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords)
{
	BeforeDraw();
	const std::tuple<VkBuffer, VkDeviceSize, void*> empty(*m_emptyBuffer, 0, nullptr);
	CVulkanSmartBuffer& vertexBuffer = m_activeCommandBuffer->GetVertexBuffer();
	const auto vertexInfo = vertices.empty() ? empty : vertexBuffer.Allocate(vertices.size() * sizeof(CVector3f));
	const auto normalsInfo = normals.empty() ? empty : vertexBuffer.Allocate(normals.size() * sizeof(CVector3f));
	const auto texCoordInfo = texCoords.empty() ? empty : vertexBuffer.Allocate(texCoords.size() * sizeof(CVector2f));
	memcpy(std::get<void*>(vertexInfo), vertices.data(), vertices.size() * sizeof(CVector3f));
	memcpy(std::get<void*>(normalsInfo), normals.data(), normals.size() * sizeof(CVector3f));
	memcpy(std::get<void*>(texCoordInfo), texCoords.data(), texCoords.size() * sizeof(CVector2f));
	static const std::map<RenderMode, VkPrimitiveTopology> topologyMap = {
		{ RenderMode::LINE_LOOP, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP },
		{ RenderMode::LINES, VK_PRIMITIVE_TOPOLOGY_LINE_LIST },
		{ RenderMode::TRIANGLE_STRIP, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP },
		{ RenderMode::TRIANGLES, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
	};
	m_pipelineHelper.SetTopology(topologyMap.at(mode));
	m_pipelineHelper.Bind(*m_activeCommandBuffer);

	const VkBuffer buffers[] = { std::get<0>(vertexInfo),  std::get<0>(normalsInfo),  std::get<0>(texCoordInfo) };
	const VkDeviceSize offsets[] = { std::get<1>(vertexInfo),  std::get<1>(normalsInfo),  std::get<1>(texCoordInfo) };
	
	vkCmdBindVertexBuffers(*m_activeCommandBuffer, 0, 3, buffers, offsets);

	vkCmdDraw(*m_activeCommandBuffer, vertices.size(), 1, 0, 0);
}

void CVulkanRenderer::RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords)
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
	m_matrixManager.Rotate(static_cast<float>(angle), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
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
	m_matrixManager.LookAt(position, direction, -up, true);
}

void CVulkanRenderer::SetTexture(const Path& texture, bool forceLoadNow /*= false*/, int flags /*= 0*/)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, flags);
}

void CVulkanRenderer::SetTexture(const Path& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, nullptr, flags);
}

void CVulkanRenderer::SetTexture(const Path& texture, const std::vector<sTeamColor> * teamcolor, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, TextureSlot::eDiffuse, teamcolor, flags);
}


void CVulkanRenderer::SetTexture(ICachedTexture const& texture, TextureSlot slot /*= TextureSlot::eDiffuse*/)
{
	auto& vulkanTexture = reinterpret_cast<CVulkanCachedTexture const&>(texture);
	if (!vulkanTexture) return;
	m_descriptorSetManager.SetTexture(&vulkanTexture, m_activeCommandBuffer ? *m_activeCommandBuffer : (VkCommandBuffer)VK_NULL_HANDLE, m_pipelineHelper.GetLayout());
}

void CVulkanRenderer::RenderToTexture(std::function<void() > const& func, ICachedTexture & tex, unsigned int width, unsigned int height)
{
	auto& texture = reinterpret_cast<CVulkanCachedTexture&>(tex);
	if (!m_serviceRenderPass)
	{
		m_serviceRenderPass = CreateRenderPass(VK_FORMAT_R8G8B8A8_UNORM);
		m_serviceRenderPass.SetDevice(m_device);
	}
	std::unique_ptr<CCommandBufferWrapper> commandBuffer = std::make_unique<CCommandBufferWrapper>(m_commandPool, *this);
	commandBuffer->WaitFence();
	const VkImageView view = texture.GetImageView();
	VkFramebuffer frameBuffer;
	const VkFramebufferCreateInfo framebuffer_create_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, nullptr, 0, m_serviceRenderPass, 1, &view, width, height, 1 };
	VkResult result = vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &frameBuffer);
	LOG_VK_RESULT(result, "Failed to create framebuffer");
	commandBuffer->SetFrameBuffer(frameBuffer);
	CCommandBufferWrapper* oldCommandBuffer = m_activeCommandBuffer;
	const VkImage oldImage = m_currentImage;
	m_currentImage = texture;
	m_activeCommandBuffer = commandBuffer.get();
	m_activeCommandBuffer->Begin();
	VkClearValue clearValue;
	clearValue.color = VkClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f };
	const VkRenderPassBeginInfo render_pass_begin_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, nullptr, m_serviceRenderPass, frameBuffer,{ { 0, 0 }, {width, height} }, 1, &clearValue };
	vkCmdBeginRenderPass(*m_activeCommandBuffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	const VkViewport viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	vkCmdSetViewport(*m_activeCommandBuffer, 0, 1, &viewport);
	const VkRect2D scissor = { { 0, 0 }, { width, height } };
	vkCmdSetScissor(*m_activeCommandBuffer, 0, 1, &scissor);
	m_pipelineHelper.SetRenderPass(m_serviceRenderPass);
	m_pipelineHelper.Bind(*m_activeCommandBuffer);
	m_shaderManager.CommitUniforms(commandBuffer->GetUniformBuffer(), true);
	m_descriptorSetManager.SetShaderProgram(m_shaderManager.GetActiveProgram(), *commandBuffer, m_pipelineHelper.GetLayout());
	m_descriptorSetManager.BindAll(*m_activeCommandBuffer, m_pipelineHelper.GetLayout());
	m_matrixManager.SaveMatrices();
	m_matrixManager.SetOrthographicProjection(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	m_matrixManager.ResetModelView();
	func();
	m_matrixManager.RestoreMatrices();
	m_activeCommandBuffer->GetVertexBuffer().Commit();
	vkCmdEndRenderPass(*m_activeCommandBuffer);
	m_activeCommandBuffer->End();

	const VkCommandBuffer buffer = *m_activeCommandBuffer;
	const VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	const VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 0, nullptr, &mask, 1, &buffer, 0, nullptr };
	result = vkQueueSubmit(m_graphicsQueue, 1, &submit_info, m_activeCommandBuffer->GetFence());
	LOG_VK_RESULT(result, "Cannot submit service command buffer to queue");

	auto& smartBuffer = commandBuffer->GetUniformBuffer();
	auto buffers = smartBuffer.GetAllBuffers();
	for (VkBuffer uniformBuffer : buffers)
	{
		auto sets = m_descriptorSetManager.GetSetsWithUniformBuffer(uniformBuffer);
		for (auto set : sets)
		{
			if (std::find_if(m_descriptorsToDestroy.begin(), m_descriptorsToDestroy.end(), [set](std::pair<VkDescriptorSet, int> const& p) { return p.first == set; }) == m_descriptorsToDestroy.end())
			{
				m_descriptorsToDestroy.push_back(std::make_pair(set, RESOURCE_DELAY_FRAMES));
			}
		}
	}

	m_commandBuffersToDestroy.push_back(std::make_pair(std::move(commandBuffer), BUFFER_DELAY_FRAMES));
	m_activeCommandBuffer = oldCommandBuffer;
	m_currentImage = oldImage;
	m_pipelineHelper.SetRenderPass(m_renderPass);
}

std::unique_ptr<ICachedTexture> CVulkanRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type /*= CachedTextureType::RGBA*/)
{
	auto texture = std::make_unique<CVulkanCachedTexture>(*this);
	texture->Init(width, height, *m_memoryManager, type, TEXTURE_HAS_ALPHA);
	if (data)
	{
		BeginServiceCommandBuffer();
		texture->Upload(data, *m_memoryManager, *m_serviceCommandBuffer);
	}
	return std::move(texture);
}

ICachedTexture* CVulkanRenderer::GetTexturePtr(const Path& texture) const
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

std::unique_ptr<IVertexBuffer> CVulkanRenderer::CreateVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/, size_t size /*= 0*/, bool temp /*= false*/)
{
	if (temp)
	{
		return std::make_unique<CTempVulkanVertexBuffer>(this, vertex, normals, texcoords, size);
	}
	else
	{
		BeginServiceCommandBuffer();
		return std::make_unique<CVulkanVertexBuffer>(this, *m_serviceCommandBuffer, vertex, normals, texcoords, size);
	}
}

void CVulkanRenderer::BeginServiceCommandBuffer()
{
	if (!m_serviceBufferIsActive)
	{
		m_serviceCommandBuffer->Begin();
		m_serviceBufferIsActive = true;
	}
}

std::unique_ptr<IOcclusionQuery> CVulkanRenderer::CreateOcclusionQuery()
{
	return std::make_unique<CVulkanOcclusionQuery>();
}

std::string CVulkanRenderer::GetName() const
{
	return "Vulkan";
}

bool CVulkanRenderer::SupportsFeature(Feature) const
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
	constexpr std::array<const char*, 1> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	std::vector<float> queue_priorities = { 1.0f };
	const VkDeviceQueueCreateInfo queue_create_info = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, m_graphicsQueueFamilyIndex, static_cast<uint32_t>(queue_priorities.size()), queue_priorities.data() };
	const VkDeviceCreateInfo device_create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0, 1, &queue_create_info, 0, nullptr, static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(), nullptr };
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
	const VkExtent2D extent = GetSurfaceExtent(surfaceCapabilities);
	const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	const VkSurfaceTransformFlagBitsKHR transformFlags = (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) 
		? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR 
		: surfaceCapabilities.currentTransform;
	auto presentMode = SelectPresentMode(present_modes, { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR });

	const VkSwapchainCreateInfoKHR swap_chain_create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, nullptr, 0, m_surface, image_count, surfaceFormat.format, surfaceFormat.colorSpace,
		extent, 1, imageUsageFlags, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, transformFlags, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, presentMode, VK_TRUE, m_swapchain };

	VkSwapchainKHR swapchain;
	result = vkCreateSwapchainKHR(m_device, &swap_chain_create_info, nullptr, &swapchain);
	CHECK_VK_RESULT(result, "Failed to create swapchain");
	m_swapchain.Init(swapchain, m_device, extent, surfaceFormat.format, this);
}

void CVulkanRenderer::CreateCommandBuffers()
{
	const VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_graphicsQueueFamilyIndex };
	const VkResult result = vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool);
	CHECK_VK_RESULT(result, "Cannot create command pool");

	m_commandBuffers.reserve(COMMAND_BUFFERS_COUNT);
	for (uint32_t i = 0; i < COMMAND_BUFFERS_COUNT; ++i)
	{
		m_commandBuffers.emplace_back(m_commandPool, *this);
	}
	m_serviceCommandBuffer = std::make_unique<CCommandBufferWrapper>(m_commandPool, *this);
	m_serviceCommandBuffer->WaitFence();
}

VkRenderPass CVulkanRenderer::CreateRenderPass(VkFormat format, VkFormat depthFormat)
{
	const VkAttachmentDescription attachmentDescriptions[] = {
		{ 0, format, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR },
		{ 0, depthFormat, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
	};
	const VkAttachmentReference color_attachment_references[] = { { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
	const VkAttachmentReference depth_attachment_references[] = { { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } };
	const uint32_t count = depthFormat ? 2 : 1;
	const VkSubpassDescription subpass_descriptions[] = { { 0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, nullptr, 1, color_attachment_references, nullptr, (count == 2 ? depth_attachment_references : nullptr), 0, nullptr } };
	const std::vector<VkSubpassDependency> dependencies = {
		{ VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_DEPENDENCY_BY_REGION_BIT },
		{ 0, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT },
	};
	
	const VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0, count, attachmentDescriptions, 1, subpass_descriptions, static_cast<uint32_t>(dependencies.size()), dependencies.data() };
	VkRenderPass renderPass;
	const VkResult result = vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &renderPass);
	CHECK_VK_RESULT(result, "cannot create render pass");
	return renderPass;
}

void CVulkanRenderer::InitFramebuffer(bool useDepth)
{
	VkFramebuffer buffer;
	const VkImageView views[] = { m_swapchain.GetImageView(m_currentImageIndex), m_swapchain.GetDepthTexture().GetImageView() };
	const VkExtent2D size = m_swapchain.GetExtent();
	const VkFramebufferCreateInfo framebuffer_create_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, nullptr, 0, m_renderPass, useDepth ? 2u : 1u, views, size.width, size.height, 1 };
	const VkResult result = vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &buffer);
	LOG_VK_RESULT(result, "Failed to create framebuffer");
	m_activeCommandBuffer->SetFrameBuffer(buffer);
}

template <class T>
void FreeResourceType(std::deque<std::pair<T, int>> &resources, VkDevice device, VKAPI_ATTR void (VKAPI_CALL*deleter)(VkDevice, T, const VkAllocationCallbacks *), bool force)
{
	for (auto& res : resources) --res.second;
	while (!resources.empty() && (resources.front().second == 0 || force))
	{
		deleter(device, resources.front().first, nullptr);
		resources.pop_front();
	}
}

void CVulkanRenderer::FreeResources(bool force)
{
	for (auto& buffer : m_commandBuffersToDestroy) --buffer.second;
	while (!m_commandBuffersToDestroy.empty() && ((m_commandBuffersToDestroy.front().second == 0) || force))
	{
		m_commandBuffersToDestroy.pop_front();
	}
	for (auto& descriptor : m_descriptorsToDestroy) --descriptor.second;
	while (!m_descriptorsToDestroy.empty() && ((m_descriptorsToDestroy.front().second == 0) || force))
	{
		m_descriptorSetManager.DeleteSet(m_descriptorsToDestroy.front().first);
		m_descriptorsToDestroy.pop_front();
	}
	FreeResourceType(m_samplersToDestroy, m_device, vkDestroySampler, force);
	FreeResourceType(m_imageViewsToDestroy, m_device, vkDestroyImageView, force);
	FreeResourceType(m_imagesToDestroy, m_device, vkDestroyImage, force);
	FreeResourceType(m_buffersToDestroy, m_device, vkDestroyBuffer, force);
	m_memoryManager->FreeResources();
}

void CPipelineHelper::Init(VkDevice device)
{
	m_pipelineLayout.Destroy();
	const VkResult result = vkCreatePipelineLayout(device, &layout_create_info, nullptr, &m_pipelineLayout);
	m_pipelineLayout.SetDevice(device);
	pipeline_create_info.layout = m_pipelineLayout;
	CHECK_VK_RESULT(result, "Cannot create pipeline layout");
	m_device = device;
}


VkPipeline CPipelineHelper::GetPipeline()
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


void CPipelineHelper::Bind(VkCommandBuffer commandBuffer)
{
	const VkPipeline newPipeline = GetPipeline();
	if (newPipeline != m_currentPipeline || commandBuffer != m_currentBuffer)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, newPipeline);
		m_currentPipeline = newPipeline;
		m_currentBuffer = commandBuffer;
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
		vertex_binding_descriptions.push_back({ static_cast<uint32_t>(vertex_binding_descriptions.size()), attrib.size, attrib.perInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX });
		vertex_attribute_descriptions.push_back({ attrib.pos, vertex_binding_descriptions.back().binding, attrib.format, 0 });
	}
	vertex_input_state_create_info.vertexBindingDescriptionCount = vertex_binding_descriptions.size();
	vertex_input_state_create_info.pVertexBindingDescriptions = vertex_binding_descriptions.data();
	vertex_input_state_create_info.vertexAttributeDescriptionCount = vertex_attribute_descriptions.size();
	vertex_input_state_create_info.pVertexAttributeDescriptions = vertex_attribute_descriptions.data();
	m_currentKey.attribs = attribs;
}

size_t CPipelineHelper::AddVertexAttribute(VertexAttrib attrib)
{
	auto attribs = m_currentKey.attribs;
	auto it = std::find_if(attribs.begin(), attribs.end(), [&attrib](const VertexAttrib & attr) {
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

void CPipelineHelper::RemoveVertexAttribute(uint32_t pos)
{
	auto attribs = m_currentKey.attribs;
	attribs.erase(std::remove_if(attribs.begin(), attribs.end(), [pos](const VertexAttrib & attrib) { return attrib.pos == pos; }), attribs.end());
	SetVertexAttributes(attribs);
}

void CPipelineHelper::SetDescriptorLayout(const VkDescriptorSetLayout * layouts, uint32_t count)
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

void CPipelineHelper::SetBlending(bool enable)
{
	color_blend_attachment_state.blendEnable = enable ? VK_TRUE : VK_FALSE;
	m_currentKey.blending = enable;
}

void CPipelineHelper::SetDepthParams(bool test, bool write)
{
	depthStencil.depthTestEnable = test ? VK_TRUE : VK_FALSE;
	depthStencil.depthWriteEnable = write ? VK_TRUE : VK_FALSE;
	m_currentKey.depthTest = test;
	m_currentKey.depthWrite = test;
}

void CPipelineHelper::SetRenderPass(VkRenderPass pass)
{
	pipeline_create_info.renderPass = pass;
	m_currentKey.renderPass = pass;
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

CCommandBufferWrapper::CCommandBufferWrapper(VkCommandPool pool, CVulkanRenderer & renderer)
	: m_device(renderer.GetDevice()), m_pool(pool)
	, m_vertexBuffer(sizeof(float) * 1000, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, renderer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	, m_uniformBuffer(sizeof(float) * 1000, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, renderer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
{
	m_imageAvailibleSemaphore.SetDevice(m_device);
	m_renderingFinishedSemaphore.SetDevice(m_device);
	m_frameBuffer.SetDevice(m_device);
	m_fence.SetDevice(m_device);

	const VkSemaphoreCreateInfo semaphore_create_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0 };
	if (vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_imageAvailibleSemaphore) || vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_renderingFinishedSemaphore))
	{
		throw std::runtime_error("Cannot create semaphores");
	}

	const VkFenceCreateInfo fence_create_info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, VK_FENCE_CREATE_SIGNALED_BIT };
	VkResult result = vkCreateFence(m_device, &fence_create_info, nullptr, &m_fence);
	CHECK_VK_RESULT(result, "Cannot create fence");

	const VkCommandBufferAllocateInfo allocateBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr, pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1 };
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
	const VkResult result = vkWaitForFences(m_device, 1, &m_fence, VK_FALSE, UINT64_MAX);
	LOG_VK_RESULT(result, "Waiting on fence takes too long");
	vkResetFences(m_device, 1, &m_fence);
}

void CCommandBufferWrapper::Begin()
{
	const VkCommandBufferBeginInfo beginBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr };
	const VkResult result = vkBeginCommandBuffer(m_commandBuffer, &beginBufferInfo);
	LOG_VK_RESULT(result, L"cannot begin command buffer");
}

void CCommandBufferWrapper::End()
{
	m_vertexBuffer.Commit();
	m_uniformBuffer.Commit();
	const VkResult result = vkEndCommandBuffer(m_commandBuffer);
	LOG_VK_RESULT(result, L"cannot end command buffer");
}

void CSwapchainWrapper::Init(VkSwapchainKHR swapchain, VkDevice device, VkExtent2D extent, VkFormat format, CVulkanRenderer * renderer)
{
	m_renderer = renderer;
	m_swapchain.Destroy();
	for (auto view : m_imageViews) renderer->DestroyImage(nullptr, VK_NULL_HANDLE, view);
	m_imageViews.clear();
	m_images.clear();
	m_swapchain = swapchain;
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
	}
	m_extent = extent;
	m_format = format;

	m_depthTexture = std::make_unique<CVulkanCachedTexture>(*renderer);
	m_depthTexture->Init(m_extent.width, m_extent.height, renderer->GetMemoryManager(), CachedTextureType::DEPTH);
}

CSwapchainWrapper::~CSwapchainWrapper()
{
	if (m_renderer)
	{
		for (auto view : m_imageViews)
		{
			m_renderer->DestroyImage(nullptr, VK_NULL_HANDLE, view);
		}
	}
}

CVulkanCachedTexture::CVulkanCachedTexture(CVulkanRenderer & renderer)
	: m_device(renderer.GetDevice()), m_renderer(&renderer)
{
}

CVulkanCachedTexture::~CVulkanCachedTexture()
{
	m_renderer->DestroyImage(this);
}

std::pair<VkImage, std::unique_ptr<CVulkanMemory>> CVulkanCachedTexture::CreateTexture(bool deviceLocal, CVulkanMemoryManager & memoryManager)
{
	VkImage image;
	const VkImageUsageFlags usageFlags = m_usageFlags | (deviceLocal ? VK_IMAGE_USAGE_TRANSFER_DST_BIT : VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
	const VkImageCreateInfo image_create_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,  nullptr, 0, VK_IMAGE_TYPE_2D, m_format, m_extent, 1, 1, VK_SAMPLE_COUNT_1_BIT,
		m_format == VK_FORMAT_D32_SFLOAT ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR, usageFlags, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_IMAGE_LAYOUT_PREINITIALIZED };
	VkResult result = vkCreateImage(m_device, &image_create_info, nullptr, &image);

	VkMemoryRequirements image_memory_requirements;
	vkGetImageMemoryRequirements(m_device, image, &image_memory_requirements);

	const VkMemoryPropertyFlags memoryProperties = deviceLocal ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	std::unique_ptr<CVulkanMemory> memory = memoryManager.Allocate(image_memory_requirements, memoryProperties);
	if (!memory)
	{
		LogWriter::WriteLine("Cannot allocate memory for image");
		return std::make_pair(image, std::move(memory));
	}
	result = vkBindImageMemory(m_device, image, *memory, memory->GetOffset());
	LOG_VK_RESULT(result, "Cannot bind memory to an image");
	return std::make_pair(image, std::move(memory));
}

void CVulkanCachedTexture::Init(uint32_t width, uint32_t height, CVulkanMemoryManager & memoryManager, CachedTextureType type, int flags)
{
	if (width == 0 || height == 0) return;
	m_format = GetTextureFormat(type, flags);
	m_extent = { width, height, 1 };
	m_components = type == CachedTextureType::ALPHA ? 1 : (flags & TEXTURE_HAS_ALPHA ? 4 : 3);
	m_usageFlags = type == CachedTextureType::DEPTH ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : ((type == CachedTextureType::RENDER_TARGET ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0) | VK_IMAGE_USAGE_SAMPLED_BIT);
	m_usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	std::tie(m_image, m_memory) = CreateTexture(true, memoryManager);

	const VkComponentSwizzle swizzle = type == CachedTextureType::ALPHA ? VK_COMPONENT_SWIZZLE_ZERO : VK_COMPONENT_SWIZZLE_IDENTITY;
	const VkComponentMapping mapping = { swizzle, swizzle, swizzle, type == CachedTextureType::ALPHA ? VK_COMPONENT_SWIZZLE_R : VK_COMPONENT_SWIZZLE_IDENTITY };
	const VkImageAspectFlags aspectFlags = type == CachedTextureType::DEPTH ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	const VkImageViewCreateInfo image_view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0, m_image, VK_IMAGE_VIEW_TYPE_2D, m_format,
	mapping, { aspectFlags, 0, 1, 0, 1 } };
	VkResult result = vkCreateImageView(m_device, &image_view_create_info, nullptr, &m_imageView);
	LOG_VK_RESULT(result, "Cannot create imageView");

	const VkSamplerAddressMode wrapMode = flags & TEXTURE_NO_WRAP ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
	const VkSamplerCreateInfo sampler_create_info = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, nullptr, 0, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, wrapMode,
		wrapMode, wrapMode, 0.0f, VK_FALSE, 1.0f, VK_FALSE, VK_COMPARE_OP_ALWAYS, 0.0f,  0.0f, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, VK_FALSE };
	result = vkCreateSampler(m_device, &sampler_create_info, nullptr, &m_sampler);
	LOG_VK_RESULT(result, "Cannot create sampler");
}


void CVulkanCachedTexture::TransferImageLayout(VkImage stageImage, VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	const VkImageSubresourceRange imageRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED, stageImage, imageRange };
	if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;;
	}
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void CVulkanCachedTexture::Upload(const void * data, CVulkanMemoryManager & memoryManager, VkCommandBuffer commandBuffer)
{
	VkImage stageImage;
	std::unique_ptr<CVulkanMemory> stageMemory;
	std::tie(stageImage, stageMemory) = CreateTexture(false, memoryManager);
		
	void *staging_buffer_memory_pointer;
	VkResult result = vkMapMemory(m_device, *stageMemory, stageMemory->GetOffset(), stageMemory->GetSize(), 0, &staging_buffer_memory_pointer);
	LOG_VK_RESULT(result, "Cannot map memory for image");

	VkImageSubresource subresource = {};
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource.mipLevel = 0;
	subresource.arrayLayer = 0;

	VkSubresourceLayout stagingImageLayout;
	vkGetImageSubresourceLayout(m_device, m_image, &subresource, &stagingImageLayout);
	if (stagingImageLayout.rowPitch == m_extent.width * m_components)
	{
		memcpy(staging_buffer_memory_pointer, data, static_cast<size_t>(stageMemory->GetSize()));
	}
	else
	{
		const uint8_t* dstBytes = reinterpret_cast<const uint8_t*>(staging_buffer_memory_pointer);
		const uint8_t* srcBytes = reinterpret_cast<const uint8_t*>(data);

		for (uint32_t y = 0; y < m_extent.height; y++) 
		{
			memcpy((void*)&dstBytes[y * stagingImageLayout.rowPitch], (void*)&srcBytes[y * m_extent.width * m_components], m_extent.width * m_components);
		}
	}
	vkUnmapMemory(m_device, *stageMemory);

	TransferImageLayout(stageImage, commandBuffer, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	TransferImageLayout(m_image, commandBuffer, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkImageSubresourceLayers layers = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	VkImageCopy imageCopy = { layers, VkOffset3D{0, 0, 0}, layers, VkOffset3D{0, 0, 0}, m_extent };
	vkCmdCopyImage(commandBuffer, stageImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
	m_renderer->DestroyImage(nullptr, stageImage);
	TransferImageLayout(m_image, commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

CVulkanVertexBuffer::CVulkanVertexBuffer(CVulkanRenderer * renderer, VkCommandBuffer commandBuffer, const float * vertex, const float * normals, const float * texcoords, size_t size)
	: m_size((vertex ? size * 3 * sizeof(float) : 0) + (normals ? size * 3 * sizeof(float) : 0) + (texcoords ? size * 2 * sizeof(float) : 0))
	, m_vertexCache(m_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, *renderer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	, m_renderer(renderer)
{
	m_offsets[0] = 0;
	m_offsets[1] = normals ? (vertex ? size * 3 * sizeof(float) : 0) : 0;
	m_offsets[2] = texcoords ? (vertex ? size * 3 * sizeof(float) : 0) + (normals ? size * 3 * sizeof(float) : 0) : 0;
	if (m_size == 0) return;
	std::vector<char> data(static_cast<size_t>(m_size));
	if (vertex) memcpy(data.data() + m_offsets[0], vertex, size * 3 * sizeof(float));
	if (normals) memcpy(data.data() + m_offsets[1], normals, size * 3 * sizeof(float));
	if (texcoords) memcpy(data.data() + m_offsets[2], texcoords, size * 2 * sizeof(float));
	m_vertexCache.UploadStaged(data.data(), data.size(), commandBuffer);
}

void CVulkanVertexBuffer::SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize)
{
	if (!indexPtr || indexesSize == 0) return;
	m_indexCache = std::make_unique<CVulkanVertexAttribCache>(indexesSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, *m_renderer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	m_indexCache->Upload(indexPtr, indexesSize * sizeof(float));
}

void CVulkanVertexBuffer::Bind() const
{
	auto& pipelineHelper = m_renderer->GetPipelineHelper();
	pipelineHelper.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineHelper.Bind(m_renderer->GetCommandBuffer());
	VkBuffer buffers[3];
	for (size_t i = 0; i < sizeof(buffers) / sizeof(buffers[0]); ++i)
	{
		buffers[i] = ((m_offsets[i] != 0 || i == 0) && m_vertexCache) ? m_vertexCache : m_renderer->GetEmptyBuffer();
	}
	vkCmdBindVertexBuffers(m_renderer->GetCommandBuffer(), 0, 3, buffers, m_offsets);
	if (m_indexCache)
	{
		vkCmdBindIndexBuffer(m_renderer->GetCommandBuffer(), *m_indexCache, 0, VK_INDEX_TYPE_UINT32);
	}
}

void CVulkanVertexBuffer::DrawIndexes(size_t begin, size_t count)
{
	if (count == 0) return;
	m_renderer->BeforeDraw();
	vkCmdDrawIndexed(m_renderer->GetCommandBuffer(), count, 1, begin, 0, 0);
}

void CVulkanVertexBuffer::DrawAll(size_t count)
{
	if (count == 0) return;
	m_renderer->BeforeDraw();
	vkCmdDraw(m_renderer->GetCommandBuffer(), count, 1, 0, 0);
}

void CVulkanVertexBuffer::DrawInstanced(size_t size, size_t instanceCount)
{
	if (size == 0 || instanceCount == 0) return;
	m_renderer->BeforeDraw();
	vkCmdDraw(m_renderer->GetCommandBuffer(), size, instanceCount, 0, 0);
}

void CVulkanVertexBuffer::UnBind() const
{
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

void CVulkanDescriptorSetManager::SetShaderProgram(const CVulkanShaderProgram * program, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
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

void CVulkanDescriptorSetManager::SetTexture(const CVulkanCachedTexture * texture, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
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

VkDescriptorSet CVulkanDescriptorSetManager::GetTextureDescriptor(const CVulkanCachedTexture * texture)
{
	auto it = m_textureSets.find(texture);
	return it == m_textureSets.end() ? VK_NULL_HANDLE : it->second;
}

void CVulkanDescriptorSetManager::DeleteSet(VkDescriptorSet set)
{
	vkFreeDescriptorSets(m_device, m_desciptorPool, 1, &set);
	auto it = std::find_if(m_textureSets.begin(), m_textureSets.end(), [set](std::pair<const CVulkanCachedTexture*, VkDescriptorSet> const& pair) {return pair.second == set; });
	if (it != m_textureSets.end()) m_textureSets.erase(it);
	auto it2 = std::find_if(m_programDescriptorSets.begin(), m_programDescriptorSets.end(), [set](std::pair<ProgramDescriptorSetKey, VkDescriptorSet> const& pair) {return pair.second == set; });
	if (it2 != m_programDescriptorSets.end()) m_programDescriptorSets.erase(it2);
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
