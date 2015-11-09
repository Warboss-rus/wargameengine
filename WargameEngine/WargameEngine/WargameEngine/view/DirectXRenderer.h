#pragma once
#include "IRenderer.h"
#include "IViewHelper.h"
#include <d3d11.h>
#include "TextureManager.h"
#include <atlcomcli.h>
#include <DirectXMath.h>

class CShaderManagerDirectX;

class CDirectXRenderer : public IRenderer, public IViewHelper
{
public:
	CDirectXRenderer(ID3D11Device *dev = nullptr, ID3D11DeviceContext *devcon = nullptr, HWND hWnd = NULL);

	virtual void RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords) override;
	virtual void RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords) override;

	virtual void SetColor(const float r, const float g, const float b) override;
	virtual void SetColor(const int r, const int g, const int b) override;
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
	virtual void ResetViewMatrix() override;
	virtual void LookAt(CVector3d const& position, CVector3d const& direction, CVector3d const& up) override;

	virtual void SetTexture(std::string const& texture, bool forceLoadNow = false, int flags = 0) override;
	virtual void SetTexture(std::string const& texture, TextureSlot slot, int flags = 0) override;
	virtual void SetTexture(std::string const& texture, const std::vector<sTeamColor> * teamcolor, int flags = 0) override;

	virtual std::unique_ptr<ICachedTexture> RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height) override;
	virtual std::unique_ptr<ICachedTexture> CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) override;

	virtual void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess) override;

	virtual std::unique_ptr<IDrawingList> CreateDrawingList(std::function<void() > const& func) override;
	virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0) override;
	virtual std::unique_ptr<IShaderManager> CreateShaderManager() const override;

	virtual void WindowCoordsToWorldVector(int x, int y, CVector3d & start, CVector3d & end) const override;
	virtual void WorldCoordsToWindowCoords(CVector3d const& worldCoords, int& x, int& y) const override;
	virtual std::unique_ptr<IFrameBuffer> CreateFramebuffer() const override;
	virtual void EnableLight(size_t index, bool enable) override;
	virtual void SetLightColor(size_t index, LightningType type, float * values) override;
	virtual void SetLightPosition(size_t index, float* pos) override;
	virtual float GetMaximumAnisotropyLevel() const override;
	virtual void EnableVertexLightning(bool enable) override;
	virtual void GetProjectionMatrix(float * matrix) const override;
	virtual void EnableDepthTest(bool enable) override;
	virtual void EnableBlending(bool enable) override;
	virtual void SetUpViewport(CVector3d const& position, CVector3d const& target, unsigned int viewportWidth, unsigned int viewportHeight, double viewingAngle, double nearPane = 1.0, double farPane = 1000.0) override;
	virtual void RestoreViewport() override;
	virtual void EnablePolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) override;
	virtual void ClearBuffers(bool color = true, bool depth = true) override;
	virtual CTextureManager& GetTextureManager() override;
	virtual void SetUpViewport2D() override;

	virtual void ActivateTextureSlot(TextureSlot slot) override;
	virtual void UnbindTexture() override;
	virtual std::unique_ptr<ICachedTexture> CreateEmptyTexture() override;
	virtual void SetTextureAnisotropy(float value = 1.0f) override;
	virtual void UploadTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	virtual void UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, size_t size, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;

	void SetShaderManager(CShaderManagerDirectX * shaderManager);
	void SetTextureResource(ID3D11ShaderResourceView * view);
private:
	void CreateBuffer(ID3D11Buffer ** bufferPtr, void * data, size_t size);
	void CreateBuffer(ID3D11Buffer ** bufferPtr, unsigned int elementSize);
	void CreateTexture(unsigned int width, unsigned int height, int flags, const void * data, ID3D11Texture2D ** texture, ID3D11ShaderResourceView ** resourceView, bool renderTarget = false);
	void UpdateMatrices();

	CTextureManager m_textureManager;
	std::unique_ptr<CShaderManagerDirectX> m_defaultShaderManager;
	CShaderManagerDirectX * m_shaderManager;
	ID3D11Device *m_dev;
	ID3D11DeviceContext *m_devcon;
	HWND m_hWnd;
	unsigned int m_activeTextureSlot;

	CComPtr<ID3D11Buffer> m_vertexBuffer;
	CComPtr<ID3D11Buffer> m_normalsBuffer;
	CComPtr<ID3D11Buffer> m_texCoordBuffer;

	std::vector<DirectX::XMMATRIX> m_viewMatrices;
	std::vector<DirectX::XMMATRIX> m_projectionMatrices;
	float m_anisotropyLevel = 0.0f;
};