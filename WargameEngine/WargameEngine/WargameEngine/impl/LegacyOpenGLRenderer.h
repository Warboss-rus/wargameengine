#pragma once
#include "IOpenGLRenderer.h"
#include "ShaderManagerLegacyGL.h"

using wargameEngine::view::IVertexBuffer;
using wargameEngine::view::ICachedTexture;
using wargameEngine::view::IShaderManager;

class CLegacyGLRenderer : public IOpenGLRenderer
{
public:
	using TextureMipMaps = wargameEngine::view::TextureMipMaps;

	CLegacyGLRenderer();

	virtual void RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords) override;
	virtual void DrawIndexed(wargameEngine::view::IVertexBuffer& buffer, size_t count, size_t begin = 0, size_t instances = 0) override;
	virtual void Draw(IVertexBuffer& buffer, size_t count, size_t begin = 0, size_t instances = 0) override;
	virtual void DrawIndirect(IVertexBuffer& buffer, const array_view<IndirectDraw>& indirectList, bool indexed) override;
	virtual void SetIndexBuffer(IVertexBuffer& buffer, const unsigned int* indexPtr, size_t indexesSize) override;
	virtual void AddVertexAttribute(IVertexBuffer& buffer, const std::string& attribute, int elementSize, size_t count, IShaderManager::Format type, const void* values, bool perInstance = false) override;

	virtual void PushMatrix() override;
	virtual void PopMatrix() override;
	virtual void Translate(const CVector3f& delta) override;
	virtual void Translate(int dx, int dy, int dz = 0) override;
	virtual void Rotate(float angle, const CVector3f& axis) override;
	virtual void Rotate(const CVector3f& rotations) override;
	virtual void Scale(float scale) override;
	virtual const float* GetViewMatrix() const override;
	virtual const float* GetModelMatrix() const override;
	virtual void SetModelMatrix(const float* matrix) override;
	virtual void LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up) override;

	virtual void SetTexture(const wargameEngine::Path& texture, bool forceLoadNow = false, int flags = 0) override;
	virtual void SetTexture(ICachedTexture const& texture, TextureSlot slot = TextureSlot::Diffuse) override;
	virtual void UnbindTexture(TextureSlot slot = TextureSlot::Diffuse) override;

	virtual void RenderToTexture(std::function<void() > const& func, ICachedTexture & texture, unsigned int width, unsigned int height) override;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) override;

	virtual void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xff) override;
	virtual void SetColor(const float * color) override;
	virtual void SetMaterial(const float * ambient, const float * diffuse, const float * specular, float shininess) override;

	virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = false) override;

	virtual std::unique_ptr<wargameEngine::view::IFrameBuffer> CreateFramebuffer() const override;

	virtual std::unique_ptr<wargameEngine::view::IOcclusionQuery> CreateOcclusionQuery() override;

	virtual wargameEngine::view::IShaderManager& GetShaderManager()  override;

	virtual void SetTextureManager(wargameEngine::view::TextureManager & textureManager) override;

	virtual void WindowCoordsToWorldVector(wargameEngine::view::IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const override;
	virtual void WorldCoordsToWindowCoords(wargameEngine::view::IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const override;
	virtual void SetNumberOfLights(size_t count) override;
	virtual void SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular) override;
	virtual float GetMaximumAnisotropyLevel() const override;
	virtual const float* GetProjectionMatrix() const override;
	virtual void EnableDepthTest(bool read, bool write) override;
	virtual void EnableColorWrite(bool rgb, bool alpha) override;
	virtual void EnableBlending(bool enable) override;
	virtual void SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane = 1.0f, float farPane = 1000.0f) override;
	virtual void EnablePolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) override;
	virtual void ClearBuffers(bool color = true, bool depth = true) override;
	virtual void DrawIn2D(std::function<void()> const& drawHandler) override;

	virtual std::unique_ptr<ICachedTexture> CreateEmptyTexture(bool cubemap = false) override;
	virtual void SetTextureAnisotropy(float value = 1.0f) override;
	virtual void UploadTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	virtual void UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	virtual void UploadCubemap(ICachedTexture & texture, TextureMipMaps const& sides, unsigned short bpp, int flags) override;
	virtual bool Force32Bits() const override;
	virtual bool ForceFlipBMP() const override;
	virtual bool ConvertBgra() const override;

	virtual std::string GetName() const override;
	virtual bool SupportsFeature(Feature feature) const override;
	virtual void EnableMultisampling(bool enable) override;
private:
	void ResetViewMatrix();
	wargameEngine::view::TextureManager* m_textureManager;
	CShaderManagerLegacyGL m_shaderManager;
	mutable float m_matrix[16];
};