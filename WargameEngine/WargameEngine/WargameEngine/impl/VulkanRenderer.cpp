#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "VulkanRenderer.h"
#include "..\LogWriter.h"

namespace
{
#define CHECK_VK_RESULT(result, message) if(result) throw std::runtime_error(message)
#define clamp(value, minV, maxV) value = min(max(value, minV), maxV)

bool CheckPhysicalDevice(VkPhysicalDevice device, uint32_t & queue_family_index)
{
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures   features;
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);
	uint32_t major_version = VK_VERSION_MAJOR(properties.apiVersion);
	/*uint32_t minor_version = VK_VERSION_MINOR(device_properties.apiVersion);
	uint32_t patch_version = VK_VERSION_PATCH(device_properties.apiVersion);*/
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

}

CVulkanRenderer::CVulkanRenderer()
{
	VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "WargameEngine", VK_MAKE_VERSION(1, 0, 0), "WargameEngine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0 };
	const std::vector<char*> instanceExtensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
	};
	VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &appInfo, 0, nullptr, static_cast<uint32_t>(instanceExtensions.size()), instanceExtensions.data() };
	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_instance);
	CHECK_VK_RESULT(result, "Cannot create vulkan instance");

	CreateDeviceAndQueues();

	m_imageAvailibleSemaphore.Create(m_device);
	m_renderingFinishedSemaphore.Create(m_device);
}

CVulkanRenderer::~CVulkanRenderer()
{
	m_imageAvailibleSemaphore.Destroy();
	m_renderingFinishedSemaphore.Destroy();
	if (m_device && m_commandPool)
	{
		vkFreeCommandBuffers(m_device, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	}
	if (m_device && m_swapchain)
	{
		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
	}
	if (m_instance && m_surface)
	{
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	}
	if (m_device)
	{
		vkDeviceWaitIdle(m_device);
		vkDestroyDevice(m_device, nullptr);
	}
	if (m_instance)
	{
		vkDestroyInstance(m_instance, nullptr);
	}
}

VkInstance CVulkanRenderer::GetInstance() const
{
	return m_instance;
}

void CVulkanRenderer::SetSurface(VkSurfaceKHR surface)
{
	m_surface = surface;
	CreateSwapchain();
	CreateCommandBuffers();
	CreateInternalFramebuffer();
}

void CVulkanRenderer::AcquireImage()
{
	VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageAvailibleSemaphore, VK_NULL_HANDLE, &m_currentImageIndex);
	if (result && result != VK_SUBOPTIMAL_KHR)
	{
		LogWriter::WriteLine("Cannot acqure image");
	}
	uint32_t count = static_cast<uint32_t>(m_commandBuffers.size());
	std::vector<VkImage> swapchainImages(count);
	result = vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, swapchainImages.data());
	if (result)
	{
		LogWriter::WriteLine(L"cannot get swapchain images");
	}
	m_currentImage = swapchainImages[m_currentImageIndex];
	const VkCommandBufferBeginInfo beginBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr };
	const VkImageSubresourceRange imageRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VkImageMemoryBarrier barrier_from_present_to_clear = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_presentQueueFamilyIndex, m_presentQueueFamilyIndex, swapchainImages[m_currentImageIndex], imageRange };
	result = vkBeginCommandBuffer(m_commandBuffers[m_currentImageIndex], &beginBufferInfo);
	if (result)
	{
		LogWriter::WriteLine(L"cannot begin command buffer");
	}
	vkCmdPipelineBarrier(m_commandBuffers[m_currentImageIndex], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_present_to_clear);
}

void CVulkanRenderer::Submit()
{
	const VkImageSubresourceRange imageRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	VkImageMemoryBarrier barrier_from_clear_to_present = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, m_presentQueueFamilyIndex, m_presentQueueFamilyIndex, m_currentImage, imageRange };
	vkCmdPipelineBarrier(m_commandBuffers[m_currentImageIndex], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_from_clear_to_present);
	VkResult result = vkEndCommandBuffer(m_commandBuffers[m_currentImageIndex]);
	if (result)
	{
		LogWriter::WriteLine(L"cannot end command buffer");
	}
	
	VkPipelineStageFlags mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 1, m_imageAvailibleSemaphore.ptr(), &mask, 1, &m_commandBuffers[m_currentImageIndex], 1, m_renderingFinishedSemaphore.ptr() };
	if (vkQueueSubmit(m_presentQueue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) 
	{
		LogWriter::WriteLine("Cannot submit buffer");
	}
}

void CVulkanRenderer::Present()
{
	VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, nullptr, 1, m_renderingFinishedSemaphore.ptr(), 1, &m_swapchain, &m_currentImageIndex, nullptr };
	VkResult result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
	if (result)
	{
		LogWriter::WriteLine(L"Present failed");
	}
}

void CVulkanRenderer::Record()
{
	ClearBuffers(true, false);
}

void CVulkanRenderer::EnableMultisampling(bool enable)
{
}

void CVulkanRenderer::WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const
{
}

void CVulkanRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const
{
}

std::unique_ptr<IFrameBuffer> CVulkanRenderer::CreateFramebuffer() const
{
	return nullptr;
}

void CVulkanRenderer::SetNumberOfLights(size_t count)
{
}

void CVulkanRenderer::SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular)
{
}

float CVulkanRenderer::GetMaximumAnisotropyLevel() const
{
	return 16.0f;
}

void CVulkanRenderer::GetProjectionMatrix(float * matrix) const
{
}

void CVulkanRenderer::EnableDepthTest(bool enable)
{
}

void CVulkanRenderer::EnableBlending(bool enable)
{
}

void CVulkanRenderer::SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane /*= 1.0f*/, float farPane /*= 1000.0f*/)
{
}

void CVulkanRenderer::DrawIn2D(std::function<void() > const& drawHandler)
{
}

void CVulkanRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
}

void CVulkanRenderer::ClearBuffers(bool color /*= true*/, bool depth /*= true*/)
{
	const VkImageSubresourceRange imageRange = { (color ? VK_IMAGE_ASPECT_COLOR_BIT : 0) | (depth ? VK_IMAGE_ASPECT_DEPTH_BIT : 0), 0, 1, 0, 1 };
	VkClearColorValue clearColor = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	vkCmdClearColorImage(m_commandBuffers[m_currentImageIndex], m_currentImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &imageRange);
}

void CVulkanRenderer::SetTextureManager(CTextureManager & textureManager)
{
}

void CVulkanRenderer::ActivateTextureSlot(TextureSlot slot)
{
}

void CVulkanRenderer::UnbindTexture()
{
}

std::unique_ptr<ICachedTexture> CVulkanRenderer::CreateEmptyTexture(bool cubemap /*= false*/)
{
	return nullptr;
}

void CVulkanRenderer::SetTextureAnisotropy(float value /*= 1.0f*/)
{
}

void CVulkanRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
}

void CVulkanRenderer::UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
}

void CVulkanRenderer::UploadCubemap(ICachedTexture & texture, TextureMipMaps const& sides, unsigned short bpp, int flags)
{
}

bool CVulkanRenderer::Force32Bits() const
{
	return false;
}

bool CVulkanRenderer::ForceFlipBMP() const
{
	return false;
}

bool CVulkanRenderer::ConvertBgra() const
{
	return false;
}

void CVulkanRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
}

void CVulkanRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
}

void CVulkanRenderer::SetColor(const float r, const float g, const float b, const float a /*= 1.0f*/)
{
}

void CVulkanRenderer::SetColor(const int r, const int g, const int b, const int a /*= UCHAR_MAX*/)
{
}

void CVulkanRenderer::SetColor(const float * color)
{
}

void CVulkanRenderer::SetColor(const int * color)
{
}

void CVulkanRenderer::PushMatrix()
{
}

void CVulkanRenderer::PopMatrix()
{
}

void CVulkanRenderer::Translate(const float dx, const float dy, const float dz)
{
}

void CVulkanRenderer::Translate(const double dx, const double dy, const double dz)
{
}

void CVulkanRenderer::Translate(const int dx, const int dy, const int dz)
{
}

void CVulkanRenderer::Rotate(const double angle, const double x, const double y, const double z)
{
}

void CVulkanRenderer::Scale(const double scale)
{
}

void CVulkanRenderer::GetViewMatrix(float * matrix) const
{
}

void CVulkanRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
}

void CVulkanRenderer::SetTexture(std::wstring const& texture, bool forceLoadNow /*= false*/, int flags /*= 0*/)
{
}

void CVulkanRenderer::SetTexture(std::wstring const& texture, TextureSlot slot, int flags /*= 0*/)
{
}

void CVulkanRenderer::SetTexture(std::wstring const& texture, const std::vector<sTeamColor> * teamcolor, int flags /*= 0*/)
{
}

std::unique_ptr<ICachedTexture> CVulkanRenderer::RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height)
{
	return nullptr;
}

std::unique_ptr<ICachedTexture> CVulkanRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type /*= CachedTextureType::RGBA*/)
{
	return nullptr;
}

ICachedTexture* CVulkanRenderer::GetTexturePtr(std::wstring const& texture) const
{
	return nullptr;
}

void CVulkanRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
}

std::unique_ptr<IDrawingList> CVulkanRenderer::CreateDrawingList(std::function<void() > const& func)
{
	return nullptr;
}

std::unique_ptr<IVertexBuffer> CVulkanRenderer::CreateVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/, size_t size /*= 0*/, bool temp /*= false*/)
{
	return nullptr;
}

std::unique_ptr<IOcclusionQuery> CVulkanRenderer::CreateOcclusionQuery()
{
	return nullptr;
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
	uint32_t graphics_queue_family_index = UINT32_MAX;
	for (uint32_t i = 0; i < num_devices; ++i)
	{
		if (CheckPhysicalDevice(physical_devices[i], graphics_queue_family_index))
		{
			m_physicalDevice = physical_devices[i];
			break;
		}
	}
	m_presentQueueFamilyIndex = graphics_queue_family_index;
	if (!m_physicalDevice)
	{
		throw std::runtime_error("Cannot find compatible physical device");
	}
	std::vector<char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	std::vector<float> queue_priorities = { 1.0f };
	VkDeviceQueueCreateInfo queue_create_info = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, graphics_queue_family_index, static_cast<uint32_t>(queue_priorities.size()), queue_priorities.data() };
	VkDeviceCreateInfo device_create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, nullptr, 0, 1, &queue_create_info, 0, nullptr, static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(), nullptr };
	result = vkCreateDevice(m_physicalDevice, &device_create_info, nullptr, &m_device);
	CHECK_VK_RESULT(result, "Cannot create virtual device");
	vkGetDeviceQueue(m_device, graphics_queue_family_index, 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, m_presentQueueFamilyIndex, 0, &m_presentQueue);
}

void CVulkanRenderer::CreateSwapchain()
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &surfaceCapabilities);
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
	VkImageUsageFlags imageUsageFlags = (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) 
		? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
		: static_cast<VkImageUsageFlags>(-1);
	VkSurfaceTransformFlagBitsKHR transformFlags = (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) 
		? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR 
		: surfaceCapabilities.currentTransform;
	auto presentMode = SelectPresentMode(present_modes, { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR });

	VkSwapchainCreateInfoKHR swap_chain_create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, nullptr, 0, m_surface, image_count, surfaceFormat.format, surfaceFormat.colorSpace,
		extent, 1, imageUsageFlags, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, transformFlags, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, presentMode, VK_TRUE, m_swapchain };

	result = vkCreateSwapchainKHR(m_device, &swap_chain_create_info, nullptr, &m_swapchain);
	CHECK_VK_RESULT(result, "Failed to create swapchain");
}

void CVulkanRenderer::CreateCommandBuffers()
{
	VkCommandPoolCreateInfo commandPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, 0, m_presentQueueFamilyIndex };
	VkResult result = vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool);
	CHECK_VK_RESULT(result, "Cannot create command pool");

	uint32_t count = 0;
	result = vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, nullptr);
	CHECK_VK_RESULT(result, "Cannot get image count");
	m_commandBuffers.resize(count);
	VkCommandBufferAllocateInfo allocateBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr, m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, count };
	result = vkAllocateCommandBuffers(m_device, &allocateBufferInfo, m_commandBuffers.data());
	CHECK_VK_RESULT(result, "Cannot create command buffers");
}

void CVulkanRenderer::CreateInternalFramebuffer()
{
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	VkAttachmentDescription attachmentDescriptions[] = { 0, format, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR };
	VkAttachmentReference color_attachment_references[] = { { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
	VkSubpassDescription subpass_descriptions[] = { { 0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, nullptr, 1, color_attachment_references, nullptr, nullptr, 0, nullptr } };
	VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0, 1, attachmentDescriptions, 1, subpass_descriptions, 0, nullptr };
	VkRenderPass pass;
	VkResult result = vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &pass);
	CHECK_VK_RESULT(result, "cannot create render pass");

	uint32_t count = m_commandBuffers.size();
	std::vector<VkImage> images(m_commandBuffers.size());
	result = vkGetSwapchainImagesKHR(m_device, m_swapchain, &count, images.data());
	CHECK_VK_RESULT(result, "Cannot get image count");
	for (size_t i = 0; i < count; ++i)
	{
		m_framebuffers.emplace_back(m_device, images[i], format, pass, 600, 600);
	}
}

CVulkanSemaphoreWrapper::~CVulkanSemaphoreWrapper()
{
	Destroy();
}

void CVulkanSemaphoreWrapper::Create(VkDevice device)
{
	m_device = device;
	VkSemaphoreCreateInfo semaphore_create_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0 };
	if (vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphore) != VK_SUCCESS)
	{
		throw std::runtime_error("Cannot create semaphore");
	}
}

void CVulkanSemaphoreWrapper::Destroy()
{
	if (m_device && m_semaphore)
	{
		vkDestroySemaphore(m_device, m_semaphore, nullptr);
		m_device = VK_NULL_HANDLE;
		m_semaphore = VK_NULL_HANDLE;
	}
}

CVulkanFrameBuffer::CVulkanFrameBuffer(VkDevice device, VkImage image, VkFormat imageFormat, VkRenderPass renderPass, uint32_t width, uint32_t height)
{
	VkImageViewCreateInfo image_view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0, image, VK_IMAGE_VIEW_TYPE_2D, imageFormat,
	{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY },{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } };
	VkResult result = vkCreateImageView(device, &image_view_create_info, nullptr, &m_imageView);
	CHECK_VK_RESULT(result, "Failed to create image view for framebuffer");

	VkFramebufferCreateInfo framebuffer_create_info = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, nullptr, 0, renderPass, 1, &m_imageView, width, height, 1 };
}
