#pragma once
#include "IOpenGLRenderer.h"
#include <vulkan/vulkan.h>
#include "VulkanShaderManager.h"

class CVulkanSemaphoreWrapper
{
public:
	~CVulkanSemaphoreWrapper();
	void Create(VkDevice device);
	void Destroy();
	operator VkSemaphore() const { return m_semaphore; }
	const VkSemaphore* ptr() const { return &m_semaphore; }
private:
	VkDevice m_device = VK_NULL_HANDLE;
	VkSemaphore m_semaphore = VK_NULL_HANDLE;
};

class CVulkanFrameBuffer
{
public:
	CVulkanFrameBuffer(VkDevice device, VkImage image, VkFormat imageFormat, VkRenderPass renderPass, uint32_t width, uint32_t height);
private:
	VkFramebuffer m_buffer;
	VkImageView m_imageView;
};

class CVulkanRenderer : public IOpenGLRenderer
{
public:
	CVulkanRenderer();
	~CVulkanRenderer();

	VkInstance GetInstance() const;
	void SetSurface(VkSurfaceKHR surface);

	void AcquireImage();
	void Submit();
	void Present();
	void Record();
	
	virtual void EnableMultisampling(bool enable) override;
	virtual void WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const override;
	virtual void WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const override;
	virtual std::unique_ptr<IFrameBuffer> CreateFramebuffer() const override;
	virtual void SetNumberOfLights(size_t count) override;
	virtual void SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular) override;
	virtual float GetMaximumAnisotropyLevel() const override;
	virtual void GetProjectionMatrix(float * matrix) const override;
	virtual void EnableDepthTest(bool enable) override;
	virtual void EnableBlending(bool enable) override;
	virtual void SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane = 1.0f, float farPane = 1000.0f) override;
	virtual void DrawIn2D(std::function<void() > const& drawHandler) override;
	virtual void EnablePolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) override;
	virtual void ClearBuffers(bool color = true, bool depth = true) override;
	virtual void SetTextureManager(CTextureManager & textureManager) override;
	virtual void ActivateTextureSlot(TextureSlot slot) override;
	virtual void UnbindTexture() override;
	virtual std::unique_ptr<ICachedTexture> CreateEmptyTexture(bool cubemap = false) override;
	virtual void SetTextureAnisotropy(float value = 1.0f) override;
	virtual void UploadTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	virtual void UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	virtual void UploadCubemap(ICachedTexture & texture, TextureMipMaps const& sides, unsigned short bpp, int flags) override;
	virtual bool Force32Bits() const override;
	virtual bool ForceFlipBMP() const override;
	virtual bool ConvertBgra() const override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords) override;
	virtual void SetColor(const float r, const float g, const float b, const float a = 1.0f) override;
	virtual void SetColor(const int r, const int g, const int b, const int a = UCHAR_MAX) override;
	virtual void SetColor(const float * color) override;
	virtual void SetColor(const int * color) override;
	virtual void PushMatrix() override;
	virtual void PopMatrix() override;
	virtual void Translate(const float dx, const float dy, const float dz) override;
	virtual void Translate(const double dx, const double dy, const double dz) override;
	virtual void Translate(const int dx, const int dy, const int dz) override;
	virtual void Rotate(const double angle, const double x, const double y, const double z) override;
	virtual void Scale(const double scale) override;
	virtual void GetViewMatrix(float * matrix) const override;
	virtual void LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up) override;
	virtual void SetTexture(std::wstring const& texture, bool forceLoadNow = false, int flags = 0) override;
	virtual void SetTexture(std::wstring const& texture, TextureSlot slot, int flags = 0) override;
	virtual void SetTexture(std::wstring const& texture, const std::vector<sTeamColor> * teamcolor, int flags = 0) override;
	virtual std::unique_ptr<ICachedTexture> RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height) override;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) override;
	virtual ICachedTexture* GetTexturePtr(std::wstring const& texture) const override;
	virtual void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess) override;
	virtual std::unique_ptr<IDrawingList> CreateDrawingList(std::function<void() > const& func) override;
	virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = false) override;
	virtual std::unique_ptr<IOcclusionQuery> CreateOcclusionQuery() override;
	virtual std::string GetName() const override;
	virtual bool SupportsFeature(Feature feature) const override;
	virtual IShaderManager& GetShaderManager() override;

private:
	void CreateDeviceAndQueues();
	void CreateSwapchain();
	void CreateCommandBuffers();
	void CreateInternalFramebuffer();

	VkInstance m_instance = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue m_presentQueue = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_commandBuffers;
	std::vector<CVulkanFrameBuffer> m_framebuffers;
	VkImage m_currentImage;
	CVulkanSemaphoreWrapper m_imageAvailibleSemaphore;
	CVulkanSemaphoreWrapper m_renderingFinishedSemaphore;
	uint32_t m_currentImageIndex = 0;
	uint32_t m_presentQueueFamilyIndex = 0;
	CVulkanShaderManager m_shaderManager;
};