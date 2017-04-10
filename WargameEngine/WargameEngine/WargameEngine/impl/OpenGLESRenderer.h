#pragma once
#include "../view/IViewHelper.h"
#include <vector>
#include "ShaderManagerOpenGLES.h"
#include "MatrixManagerGLM.h"

class CShaderManagerOpenGLES;

class COpenGLESRenderer : public IViewHelper
{
public:
	COpenGLESRenderer();

	virtual void RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords) override;

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
	virtual void LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up) override;

	virtual void SetTexture(std::wstring const& texture, bool forceLoadNow = false, int flags = 0) override;
	virtual void SetTexture(std::wstring const& texture, TextureSlot slot, int flags = 0) override;
	virtual void SetTexture(std::wstring const& texture, const std::vector<sTeamColor> * teamcolor, int flags = 0) override;
	virtual void SetTexture(ICachedTexture const& texture, TextureSlot slot = TextureSlot::eDiffuse) override;
	virtual void UnbindTexture(TextureSlot slot = TextureSlot::eDiffuse) override;

	virtual void RenderToTexture(std::function<void() > const& func, ICachedTexture & texture, unsigned int width, unsigned int height) override;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) override;
	virtual ICachedTexture* GetTexturePtr(std::wstring const& texture) const override;

	virtual void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess) override;

	virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = false) override;

	virtual std::unique_ptr<IFrameBuffer> CreateFramebuffer() const override;

	virtual std::unique_ptr<IOcclusionQuery> CreateOcclusionQuery() override;

	virtual IShaderManager& GetShaderManager() override;

	virtual void SetTextureManager(CTextureManager & textureManager) override;

	virtual void WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const override;
	virtual void WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const override;
	virtual void SetNumberOfLights(size_t count) override;
	virtual void SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular) override;
	virtual float GetMaximumAnisotropyLevel() const override;
	virtual void GetProjectionMatrix(float * matrix) const override;
	virtual void EnableDepthTest(bool enable) override;
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
	virtual bool SupportsFeature(Feature feature) const override;
	virtual std::string GetName() const override;

	void SetVersion(int version);
	void Init(int width, int height);
	void SetVrViewport(float x, float y, float width, float height, float fov);
	void SetVrViewMatrices(std::vector<float*> const& matrices);
private:
	CTextureManager* m_textureManager;
	CShaderManagerOpenGLES m_shaderManager;
	int m_version;
	CMatrixManagerGLM m_matrixManager;
	float m_color[4];
	std::unique_ptr<IShaderProgram> m_defaultProgram;
	unsigned int m_vao;
	float m_vrViewport[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	float m_vrFovOverride = 0.0f;
};