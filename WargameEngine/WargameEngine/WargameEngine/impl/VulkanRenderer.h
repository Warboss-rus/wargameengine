#pragma once
#include "IOpenGLRenderer.h"
#include <vulkan/vulkan.h>
#include "VulkanShaderManager.h"
#include "VulkanHelpers.h"
#include "MatrixManagerGLM.h"

class CVulkanRenderer;

class CCommandBufferWrapper
{
public:
	CCommandBufferWrapper(VkCommandPool pool, VkDevice device);
	~CCommandBufferWrapper();
	operator VkCommandBuffer() const { return m_commandBuffer; }
	void WaitFence();
	VkFence GetFence() const { return m_fence; }
	VkSemaphore GetImageAvailibleSemaphore() const { return m_imageAvailibleSemaphore; }
	VkSemaphore GetRenderingFinishedSemaphore() const { return m_renderingFinishedSemaphore; }
private:
	VkCommandBuffer m_commandBuffer;
	CHandleWrapper<VkSemaphore, vkDestroySemaphore> m_imageAvailibleSemaphore;
	CHandleWrapper<VkSemaphore, vkDestroySemaphore> m_renderingFinishedSemaphore;
	CHandleWrapper<VkFence, vkDestroyFence> m_fence;
	VkDevice m_device;
	VkCommandPool m_pool;
};

class CSwapchainWrapper
{
public:
	void Init(VkSwapchainKHR swapchain, VkDevice device, VkExtent2D extent, VkFormat format);
	operator VkSwapchainKHR() const { return m_swapchain; }
	void Destroy() { m_swapchain.Destroy(); m_imageViews.clear(); }
	size_t GetImagesCount() const { return m_images.size(); }
	const std::vector<VkImage>& GetImages() const { return m_images; }
	VkExtent2D GetExtent() const { return m_extent; }
	VkImageView GetImageView(size_t index) const { return m_imageViews[index]; }
	VkFormat GetFormat() const { return m_format; }
private:
	CHandleWrapper<VkSwapchainKHR, vkDestroySwapchainKHR> m_swapchain;
	std::vector<VkImage> m_images;
	std::vector<CHandleWrapper<VkImageView, vkDestroyImageView>> m_imageViews;
	VkExtent2D m_extent;
	VkFormat m_format;
};

class CPipelineHelper
{
public:
	void SetShaderProgram(CVulkanShaderProgram const& program);
	void ResetVertexAttributes();
	void AddVertexAttribute(uint32_t pos, uint32_t size, VkFormat format, bool perInstance);
	void SetDescriptorLayout(VkDescriptorSetLayout * layouts, uint32_t count = 1);
	void Destroy();
	void CreatePipeline(VkDevice device, VkRenderPass pass);
	VkPipeline GetPipeline() const { return m_pipeline; }
	VkPipelineLayout GetLayout() const { return m_pipelineLayout; }
private:
	std::vector<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, };
	VkPipelineViewportStateCreateInfo viewport_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, nullptr, 0, 1, nullptr, 1, nullptr };
	std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions;
	std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions;
	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, nullptr, 0, 0, nullptr, 0, nullptr };
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, nullptr, 0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE };
	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, nullptr, 0, VK_FALSE, VK_FALSE, 
		VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f };
	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, nullptr, 0, VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE };
	VkPipelineColorBlendAttachmentState color_blend_attachment_state = { VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT };
	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, nullptr, 0, VK_FALSE, VK_LOGIC_OP_COPY, 1, &color_blend_attachment_state, { 0.0f, 0.0f, 0.0f, 0.0f } };
	VkPipelineLayoutCreateInfo layout_create_info = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, nullptr, 0, 0, nullptr, 0, nullptr };
	VkPipelineDynamicStateCreateInfo dynamic_state_create_info = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, nullptr, 0, static_cast<uint32_t>(dynamic_states.size()), dynamic_states.data() };
	VkGraphicsPipelineCreateInfo pipeline_create_info = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0, 0/*static_cast<uint32_t>(shader_stage_create_infos.size())*/, nullptr/*shader_stage_create_infos.data()*/, 
		&vertex_input_state_create_info, &input_assembly_state_create_info, nullptr, &viewport_state_create_info, &rasterization_state_create_info, &multisample_state_create_info, nullptr, &color_blend_state_create_info, 
		&dynamic_state_create_info, m_pipelineLayout, VK_NULL_HANDLE/*m_renderPass*/, 0, VK_NULL_HANDLE, -1 };
	CHandleWrapper<VkPipelineLayout, vkDestroyPipelineLayout> m_pipelineLayout;
	CHandleWrapper<VkPipeline, vkDestroyPipeline> m_pipeline;
};

class CVulkanCachedTexture : public ICachedTexture
{
public:
	CVulkanCachedTexture(VkDevice device, VkDescriptorSet descriptorSet);
	void Init(uint32_t width, uint32_t height, VkPhysicalDevice physicalDevice);
	void Upload(const void * data, VkCommandBuffer commandBuffer);

	virtual void Bind() const override;
	virtual void UnBind() const override;
private:
	CHandleWrapper<VkImage, vkDestroyImage> m_image;
	CHandleWrapper<VkDeviceMemory, vkFreeMemory> m_memory;
	CHandleWrapper<VkImageView, vkDestroyImageView> m_imageView;
	CHandleWrapper<VkSampler, vkDestroySampler> m_sampler;
	VkDeviceSize m_size;
	VkExtent3D m_extent;
	VkDevice m_device;
	VkDescriptorSet m_descriptorSet;
};

class CVulkanVertexBuffer : public IVertexBuffer
{
public:
	CVulkanVertexBuffer(CVulkanRenderer * renderer, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandBuffer commandBuffer, const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0);
	virtual void SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize) override;
	virtual void Bind() const override;
	virtual void DrawIndexes(size_t begin, size_t count) override;
	virtual void DrawAll(size_t count) override;
	virtual void DrawInstanced(size_t size, size_t instanceCount) override;
	virtual void UnBind() const override;
	void DoBeforeDraw(std::function<void()> const& handler);
private:
	CStagedVulkanVertexAttribCache m_vertexCache;
	CStagedVulkanVertexAttribCache m_normalsCache;
	CStagedVulkanVertexAttribCache m_texcoordCache;
	std::unique_ptr<CStagedVulkanVertexAttribCache> m_indexCache;
	CVulkanRenderer * m_renderer;
	std::function<void()> m_beforeDraw;
};

class CVulkanOcclusionQuery : public IOcclusionQuery
{
public:
	virtual void Query(std::function<void() > const& handler, bool renderToScreen) override { handler(); }
	virtual bool IsVisible() const override { return true; }
};

class CVulkanRenderer : public IOpenGLRenderer
{
public:
	CVulkanRenderer(const std::vector<const char*> & instanceExtensions);
	~CVulkanRenderer();

	VkInstance GetInstance() const;
	VkDevice GetDevice() const { return m_device; }
	VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
	VkCommandBuffer GetServiceCommandBuffer() { m_serviceCommandBuffer->WaitFence(); return *m_serviceCommandBuffer; }
	void SubmitServiceCommandBuffer();
	void SetSurface(VkSurfaceKHR surface);

	void AcquireImage();
	void Present();
	VkCommandBuffer GetCommandBuffer() const { return m_commandBuffers[m_currentCommandBufferIndex]; }
	
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
	void CreateRenderPass();
	void InitFramebuffer();
	void CreateDescriptors();

	VkInstance m_instance = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue m_presentQueue = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	CSwapchainWrapper m_swapchain;
	CHandleWrapper<VkCommandPool, vkDestroyCommandPool> m_commandPool;
	CHandleWrapper<VkRenderPass, vkDestroyRenderPass> m_renderPass;
	std::vector<CCommandBufferWrapper> m_commandBuffers;
	CHandleWrapper<VkFramebuffer, vkDestroyFramebuffer> m_frameBuffer;
	std::unique_ptr<CCommandBufferWrapper> m_serviceCommandBuffer;
	CHandleWrapper<VkDescriptorSetLayout, vkDestroyDescriptorSetLayout> m_descriptorSetLayout;
	CHandleWrapper<VkDescriptorPool, vkDestroyDescriptorPool> m_desciptorPool;
	VkDescriptorSet m_descriptorSet;
	VkDebugReportCallbackEXT m_debugCallback;
	std::unique_ptr<CVulkanCachedTexture> m_emptyTexture;
	VkImage m_currentImage;
	uint32_t m_currentImageIndex = 0;
	uint32_t m_graphicsQueueFamilyIndex = 0;
	uint32_t m_presentQueueFamilyIndex = 0;
	size_t m_currentCommandBufferIndex = 0;
	CVulkanShaderManager m_shaderManager;
	CPipelineHelper m_pipelineHelper;
	std::unique_ptr<IShaderProgram> m_defaultProgram;
	std::unique_ptr<CVulkanVertexAttribCache> m_vertexBuffer;
	std::unique_ptr<CVulkanVertexAttribCache> m_normalsBuffer;
	std::unique_ptr<CVulkanVertexAttribCache> m_texCoordBuffer;
	VkViewport m_viewport;
	CTextureManager * m_textureManager = nullptr;
	CMatrixManagerGLM m_matrixManager;
};