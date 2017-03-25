#pragma once
#include "IOpenGLRenderer.h"
#include <vulkan/vulkan.h>
#include "VulkanShaderManager.h"
#include "VulkanHelpers.h"
#include "MatrixManagerGLM.h"
#include <map>
#include <deque>

class CVulkanRenderer;

class CCommandBufferWrapper
{
public:
	CCommandBufferWrapper(VkCommandPool pool, CVulkanRenderer & renderer);
	CCommandBufferWrapper(const CCommandBufferWrapper & other) = delete;
	CCommandBufferWrapper(CCommandBufferWrapper && other) = default;
	~CCommandBufferWrapper();
	operator VkCommandBuffer() const { return m_commandBuffer; }

	void WaitFence();
	void Begin();
	void End();

	VkFence GetFence() const { return m_fence; }
	VkSemaphore GetImageAvailibleSemaphore() const { return m_imageAvailibleSemaphore; }
	VkSemaphore GetRenderingFinishedSemaphore() const { return m_renderingFinishedSemaphore; }
	VkFramebuffer GetFrameBuffer() const { return m_frameBuffer; }
	void SetFrameBuffer(VkFramebuffer buffer) { m_frameBuffer = buffer; }
	CVulkanSmartBuffer& GetVertexBuffer() { return m_vertexBuffer; }
	CVulkanSmartBuffer& GetUniformBuffer() { return m_uniformBuffer; }
private:
	VkCommandBuffer m_commandBuffer;
	CHandleWrapper<VkSemaphore, vkDestroySemaphore> m_imageAvailibleSemaphore;
	CHandleWrapper<VkSemaphore, vkDestroySemaphore> m_renderingFinishedSemaphore;
	CHandleWrapper<VkFence, vkDestroyFence> m_fence;
	VkDevice m_device;
	VkCommandPool m_pool;
	CHandleWrapper<VkFramebuffer, vkDestroyFramebuffer> m_frameBuffer;
	CVulkanSmartBuffer m_vertexBuffer;
	CVulkanSmartBuffer m_uniformBuffer;
};

class CPipelineHelper
{
public:
	~CPipelineHelper() { Destroy(); }
	void SetShaderProgram(CVulkanShaderProgram const& program);
	struct VertexAttrib
	{
		uint32_t pos;
		uint32_t size; 
		VkFormat format;
		bool perInstance;
		bool operator < (VertexAttrib const& other) const { return std::tie(pos, size, format, perInstance) < std::tie(other.pos, other.size, other.format, other.perInstance); }
	};
	void SetVertexAttributes(std::vector<VertexAttrib> const& attribs);
	size_t AddVertexAttribute(VertexAttrib attrib);
	void RemoveVertexAttribute(uint32_t pos);
	void SetDescriptorLayout(const VkDescriptorSetLayout * layouts, uint32_t count = 1);
	void SetTopology(VkPrimitiveTopology topology);
	void SetBlending(bool enable);
	void SetDepthParams(bool test, bool write);
	void SetRenderPass(VkRenderPass pass);
	void Destroy();
	void Init(VkDevice device);
	VkPipeline GetPipeline();
	VkPipelineLayout GetLayout() const { return m_pipelineLayout; }
	void Bind(VkCommandBuffer commandBuffer);
private:
	CHandleWrapper<VkPipelineLayout, vkDestroyPipelineLayout> m_pipelineLayout;
	std::vector<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, };
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
	VkGraphicsPipelineCreateInfo pipeline_create_info = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0, 0/*static_cast<uint32_t>(shader_stage_create_infos.size())*/, nullptr/*shader_stage_create_infos.data()*/, 
		&vertex_input_state_create_info, &input_assembly_state_create_info, nullptr, &viewport_state_create_info, &rasterization_state_create_info, &multisample_state_create_info, &depthStencil, &color_blend_state_create_info, 
		&dynamic_state_create_info, m_pipelineLayout, VK_NULL_HANDLE/*m_renderPass*/, 0, VK_NULL_HANDLE, -1 };

	VkDevice m_device;
	struct Key
	{
		const CVulkanShaderProgram * program;
		VkDescriptorSetLayout descriptors;
		VkRenderPass renderPass;
		VkPrimitiveTopology topology;
		std::vector<VertexAttrib> attribs;
		bool blending = false;
		bool depthTest = false;
		bool depthWrite = false;
		bool operator < (Key const& other) const { return std::tie(program, descriptors, renderPass, topology, attribs, blending, depthTest, depthWrite) < 
			std::tie(other.program, other.descriptors, other.renderPass, other.topology, other.attribs, other.blending, other.depthTest, other.depthWrite); }
	};
	Key m_currentKey;
	std::map<Key, VkPipeline> m_pipelines;
	VkPipeline m_currentPipeline = VK_NULL_HANDLE;
	VkCommandBuffer m_currentBuffer = VK_NULL_HANDLE;
};

class CVulkanCachedTexture : public ICachedTexture
{
public:
	CVulkanCachedTexture(CVulkanRenderer & renderer);
	~CVulkanCachedTexture();
	void Init(uint32_t width, uint32_t height, CVulkanMemoryManager & memoryManager, CachedTextureType type = CachedTextureType::RGBA, int flags = 0);
	void Upload(const void * data, CVulkanMemoryManager & memoryManager, VkCommandBuffer commandBuffer);
	operator VkImage() const { return m_image; }
	VkImageView GetImageView() const { return m_imageView; }
	VkSampler GetSampler() const { return m_sampler; }
	VkFormat GetFormat() const { return m_format; }
	void TransferTo(VkImageLayout newLayout, VkCommandBuffer commandBuffer) { TransferImageLayout(m_image, commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, newLayout); }
private:
	static void TransferImageLayout(VkImage stageImage, VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);
	std::pair<VkImage, std::unique_ptr<CVulkanMemory>> CreateTexture(bool deviceLocal, CVulkanMemoryManager & memoryManager);
	VkImage m_image = VK_NULL_HANDLE;
	std::unique_ptr<CVulkanMemory> m_memory;
	VkImageView m_imageView = VK_NULL_HANDLE;
	VkSampler m_sampler = VK_NULL_HANDLE;
	VkExtent3D m_extent = {0, 0, 0};
	VkDevice m_device;
	uint32_t m_components = 0;
	CVulkanRenderer * m_renderer = nullptr;
	VkFormat m_format;
	VkImageUsageFlags m_usageFlags;
};

class CSwapchainWrapper
{
public:
	void Init(VkSwapchainKHR swapchain, VkDevice device, VkExtent2D extent, VkFormat format, CVulkanRenderer * renderer);
	~CSwapchainWrapper();
	operator VkSwapchainKHR() const { return m_swapchain; }
	size_t GetImagesCount() const { return m_images.size(); }
	const std::vector<VkImage>& GetImages() const { return m_images; }
	VkExtent2D GetExtent() const { return m_extent; }
	VkImageView GetImageView(size_t index) const { return m_imageViews[index]; }
	VkFormat GetFormat() const { return m_format; }
	CVulkanCachedTexture& GetDepthTexture() const { return *m_depthTexture; }
	void DestroyDepthTexture() { m_depthTexture.reset(); }
private:
	CHandleWrapper<VkSwapchainKHR, vkDestroySwapchainKHR> m_swapchain;
	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_imageViews;
	std::unique_ptr<CVulkanCachedTexture> m_depthTexture;
	VkExtent2D m_extent;
	VkFormat m_format;
	CVulkanRenderer * m_renderer;
};

class CVulkanOcclusionQuery : public IOcclusionQuery
{
public:
	virtual void Query(std::function<void() > const& handler, bool /*renderToScreen*/) override { handler(); }
	virtual bool IsVisible() const override { return true; }
};

class CVulkanDescriptorSetManager
{
public:
	void Init(VkDevice device, uint32_t poolSize);
	void SetShaderProgram(const CVulkanShaderProgram * program, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
	void SetTexture(const CVulkanCachedTexture * texture, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
	void BindAll(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
	VkDescriptorSetLayout GetProgramLayout() { return m_programDescriptorSetLayout; }
	VkDescriptorSetLayout GetTextureLayout() { return m_textureDescriptorSetLayout; }
	VkDescriptorSet GetTextureDescriptor(const CVulkanCachedTexture * texture);
	void DeleteSet(VkDescriptorSet set);
	std::vector<VkDescriptorSet> GetSetsWithUniformBuffer(VkBuffer buffer) const;
private:
	void CreatePool(uint32_t poolSize);
	CHandleWrapper<VkDescriptorSetLayout, vkDestroyDescriptorSetLayout> m_programDescriptorSetLayout;
	CHandleWrapper<VkDescriptorSetLayout, vkDestroyDescriptorSetLayout> m_textureDescriptorSetLayout;
	CHandleWrapper<VkDescriptorPool, vkDestroyDescriptorPool> m_desciptorPool;
	struct ProgramDescriptorSetKey
	{
		const CVulkanShaderProgram * program;
		VkBuffer vertexBuffer;
		VkBuffer fragmentBuffer;
		bool operator < (const ProgramDescriptorSetKey & other) const { return std::tie(program, vertexBuffer, fragmentBuffer) < std::tie(other.program, other.vertexBuffer, other.fragmentBuffer); }
	};
	std::map<ProgramDescriptorSetKey, VkDescriptorSet> m_programDescriptorSets;
	std::map<const CVulkanCachedTexture*, VkDescriptorSet> m_textureSets;
	VkDevice m_device;
	uint32_t m_poolSize;
	VkDescriptorSet m_currentProgramDescriptorSet;
	VkDescriptorSet m_currentTextureDescriptorSet;
};

class CVulkanRenderer : public IOpenGLRenderer
{
public:
	CVulkanRenderer(const std::vector<const char*> & instanceExtensions);
	~CVulkanRenderer();

	VkInstance GetInstance() const;
	VkDevice GetDevice() const { return m_device; }
	VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
	CVulkanMemoryManager& GetMemoryManager() { return *m_memoryManager; }
	void SetSurface(VkSurfaceKHR surface);
	void Resize();
	void AcquireImage();
	void Present();
	VkCommandBuffer GetCommandBuffer() const { return *m_activeCommandBuffer; }
	CPipelineHelper& GetPipelineHelper() { return m_pipelineHelper; }
	VkBuffer GetEmptyBuffer() const { return *m_emptyBuffer; }
	CVulkanSmartBuffer& GetVertexBuffer() const { return m_activeCommandBuffer->GetVertexBuffer(); }
	void BeforeDraw();
	void DestroyImage(CVulkanCachedTexture * texture, VkImage image = VK_NULL_HANDLE, VkImageView view = VK_NULL_HANDLE);
	void DestroyBuffer(VkBuffer buffer);
	
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
	virtual void UnbindTexture(TextureSlot slot = TextureSlot::eDiffuse) override;
	virtual std::unique_ptr<ICachedTexture> CreateEmptyTexture(bool cubemap = false) override;
	virtual void SetTextureAnisotropy(float value = 1.0f) override;
	virtual void UploadTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	virtual void UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	virtual void UploadCubemap(ICachedTexture & texture, TextureMipMaps const& sides, unsigned short bpp, int flags) override;
	virtual bool Force32Bits() const override;
	virtual bool ForceFlipBMP() const override;
	virtual bool ConvertBgra() const override;
	virtual void RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords) override;
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
	virtual void SetTexture(ICachedTexture const& texture, TextureSlot slot = TextureSlot::eDiffuse) override;
	virtual void RenderToTexture(std::function<void() > const& func, ICachedTexture & texture, unsigned int width, unsigned int height) override;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) override;
	virtual ICachedTexture* GetTexturePtr(std::wstring const& texture) const override;
	virtual void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess) override;
	virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = false) override;
	virtual std::unique_ptr<IOcclusionQuery> CreateOcclusionQuery() override;
	virtual std::string GetName() const override;
	virtual bool SupportsFeature(Feature feature) const override;
	virtual IShaderManager& GetShaderManager() override;

private:
	void CreateDeviceAndQueues();
	void CreateSwapchain();
	void CreateCommandBuffers();
	VkRenderPass CreateRenderPass(VkFormat format, VkFormat depthFormat = VK_FORMAT_UNDEFINED);
	void InitFramebuffer(bool useDepth);
	void FreeResources(bool force);
	void BeginServiceCommandBuffer();

	CInstanceWrapper<VkInstance, vkDestroyInstance> m_instance;
	CDestructor m_debugCallbackDestructor;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	CInstanceWrapper<VkDevice, vkDestroyDevice> m_device;
	std::unique_ptr<CVulkanMemoryManager> m_memoryManager;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue m_presentQueue = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	CDestructor m_surfaceDestructor;
	CSwapchainWrapper m_swapchain;
	CHandleWrapper<VkCommandPool, vkDestroyCommandPool> m_commandPool;
	CHandleWrapper<VkRenderPass, vkDestroyRenderPass> m_renderPass;
	CHandleWrapper<VkRenderPass, vkDestroyRenderPass> m_serviceRenderPass;
	std::vector<CCommandBufferWrapper> m_commandBuffers;
	std::unique_ptr<CCommandBufferWrapper> m_serviceCommandBuffer;
	bool m_serviceBufferIsActive = false;
	CCommandBufferWrapper * m_activeCommandBuffer = nullptr;
	CVulkanDescriptorSetManager m_descriptorSetManager;
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
	std::unique_ptr<CVulkanVertexAttribCache> m_emptyBuffer;
	VkViewport m_viewport;
	CTextureManager * m_textureManager = nullptr;
	CMatrixManagerGLM m_matrixManager;
	std::deque<std::pair<VkImage, int>> m_imagesToDestroy;
	std::deque<std::pair<VkImageView, int>> m_imageViewsToDestroy;
	std::deque<std::pair<VkSampler, int>> m_samplersToDestroy;
	std::deque<std::pair<VkDescriptorSet, int>> m_descriptorsToDestroy;
	std::deque<std::pair<VkBuffer, int>> m_buffersToDestroy;
	std::deque<std::pair<std::unique_ptr<CCommandBufferWrapper>, int>> m_commandBuffersToDestroy;
};