#pragma once
#include "../view/IViewHelper.h"
#include "ShaderManagerOpenGL.h"
#pragma warning(push)
#pragma warning(disable: 4201)
#include <mat4x4.hpp>
#pragma warning(pop)

class COpenGLRenderer : public IViewHelper
{
public:
	COpenGLRenderer();

	virtual void RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords) override;

	virtual void PushMatrix() override;
	virtual void PopMatrix() override;
	virtual void Translate(const float dx, const float dy, const float dz) override;
	virtual void Translate(const double dx, const double dy, const double dz) override;
	virtual void Translate(const int dx, const int dy, const int dz) override;
	virtual void Rotate(const double angle, const double x, const double y, const double z) override;
	virtual void SetColor(const float r, const float g, const float b, const float a = 1.0f) override;
	virtual void SetColor(const int r, const int g, const int b, const int a = UCHAR_MAX) override;
	virtual void SetColor(const float * color) override;
	virtual void SetColor(const int * color) override;
	virtual void Scale(const double scale) override;
	virtual void GetViewMatrix(float * matrix) const override;
	virtual void ResetViewMatrix() override;
	virtual void LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up) override;

	virtual void SetTexture(std::wstring const& texture, bool forceLoadNow = false, int flags = 0) override;

	virtual void SetTexture(std::wstring const& texture, TextureSlot slot, int flags = 0) override;

	virtual void SetTexture(std::wstring const& texture, const std::vector<sTeamColor> * teamcolor, int flags = 0) override;

	virtual std::unique_ptr<ICachedTexture> RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height) override;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) override;

	virtual void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess) override;

	virtual std::unique_ptr<IDrawingList> CreateDrawingList(std::function<void() > const& func) override;

	virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = false) override;

	virtual std::unique_ptr<IFrameBuffer> CreateFramebuffer() const override;

	virtual std::unique_ptr<IOcclusionQuery> CreateOcclusionQuery() override;

	virtual IShaderManager& GetShaderManager()  override;

	virtual void SetTextureManager(CTextureManager & textureManager) override;

	virtual void WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const override;
	virtual void WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const override;
	virtual void EnableLight(size_t index, bool enable) override;
	virtual void SetLightColor(size_t index, LightningType type, float * values) override;
	virtual void SetLightPosition(size_t index, float* pos) override;
	virtual float GetMaximumAnisotropyLevel() const override;
	virtual void EnableVertexLightning(bool enable) override;
	virtual void GetProjectionMatrix(float * matrix) const override;
	virtual void EnableDepthTest(bool enable) override;
	virtual void EnableBlending(bool enable) override;
	virtual void SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane = 1.0f, float farPane = 1000.0f) override;
	virtual void EnablePolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) override;
	virtual void ClearBuffers(bool color = true, bool depth = true) override;
	virtual void DrawIn2D(std::function<void()> const& drawHandler) override;

	virtual void ActivateTextureSlot(TextureSlot slot) override;
	virtual void UnbindTexture() override;
	virtual std::unique_ptr<ICachedTexture> CreateEmptyTexture() override;
	virtual void SetTextureAnisotropy(float value = 1.0f) override;
	virtual void UploadTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	virtual void UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, size_t size, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	virtual bool Force32Bits() const override;
	virtual bool ForceFlipBMP() const override;
	virtual bool ConvertBgra() const override;

	virtual std::string GetName() const override;
	virtual bool SupportsFeature(Feature feature) const override;
	void EnableMultisampling(bool enable);
private:
	void UpdateMatrices() const;
	void UpdateColor() const;
	CTextureManager* m_textureManager;
	CShaderManagerOpenGL m_shaderManager;
	std::vector<glm::mat4> m_viewMatrices;
	glm::mat4 m_projectionMatrix;
	glm::vec4 m_color;
	std::unique_ptr<IShaderProgram> m_defaultProgram;
	unsigned int m_vao;
};

class COpenGlCachedTexture : public ICachedTexture
{
public:
	COpenGlCachedTexture();
	~COpenGlCachedTexture();

	virtual void Bind() const override;
	virtual void UnBind() const override;

	operator unsigned int() const;
private:
	unsigned int m_id;
};