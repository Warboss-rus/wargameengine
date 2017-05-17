#pragma once
#include "../view/IViewHelper.h"
#include <d3d11.h>
#include "../view/TextureManager.h"
#include <atlcomcli.h>
#include <DirectXMath.h>
#include "ShaderManagerDirectX.h"

struct sLightSource
{
	float diffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float specular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float pos[3] = { 0.0f, 0.0f, 0.0f };
};

class CDirectXRenderer : public IViewHelper
{
public:
	CDirectXRenderer(HWND hWnd = NULL);
	~CDirectXRenderer();

	void RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords) override;
	 void RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords) override;
	void DrawIndexes(IVertexBuffer& buffer, size_t begin, size_t count) override;
	void DrawAll(IVertexBuffer& buffer, size_t count) override;
	void DrawInstanced(IVertexBuffer& buffer, size_t size, size_t instanceCount) override;
	void SetIndexBuffer(IVertexBuffer& buffer, const unsigned int* indexPtr, size_t indexesSize) override;
	void ForceBindVertexBuffer(IVertexBuffer& buffer) override;

	void PushMatrix() override;
	void PopMatrix() override;
	void Translate(const CVector3f& delta) override;
	void Translate(int dx, int dy, int dz) override;
	void Rotate(float angle, const CVector3f& axis) override;
	void Rotate(const CVector3f& angles) override;
	void Scale(float scale) override;
	const float* GetViewMatrix() const override;
	void LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up) override;
	
	void SetTexture(const Path& texture, bool forceLoadNow = false, int flags = 0) override;
	void SetTexture(const Path&  texture, TextureSlot slot, int flags = 0) override;
	void SetTexture(const Path&  texture, const std::vector<TeamColor> * teamcolor, int flags = 0) override;
	void SetTexture(ICachedTexture const& texture, TextureSlot slot = TextureSlot::eDiffuse) override;
	void UnbindTexture(TextureSlot slot = TextureSlot::eDiffuse) override;
	void RenderToTexture(std::function<void() > const& func, ICachedTexture & texture, unsigned int width, unsigned int height) override;
	std::unique_ptr<ICachedTexture> CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type = CachedTextureType::RGBA) override;
	ICachedTexture* GetTexturePtr(std::wstring const& texture) const override;

	void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xff) override;
	void SetColor(const float * color) override;
	void SetMaterial(const float * ambient, const float * diffuse, const float * specular, float shininess) override;

	std::unique_ptr<IVertexBuffer> CreateVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = false) override;
	std::unique_ptr<IOcclusionQuery> CreateOcclusionQuery() override;
	IShaderManager& GetShaderManager() override;

	void WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const override;
	void WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const override;
	std::unique_ptr<IFrameBuffer> CreateFramebuffer() const override;
	void SetNumberOfLights(size_t count) override;
	void SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular) override;
	float GetMaximumAnisotropyLevel() const override;
	const float* GetProjectionMatrix() const override;
	void EnableDepthTest(bool enable) override;
	void EnableBlending(bool enable) override;
	void SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane = 1.0f, float farPane = 1000.0f) override;
	void EnablePolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) override;
	void ClearBuffers(bool color = true, bool depth = true) override;
	void SetTextureManager(TextureManager & textureManager) override;
	void DrawIn2D(std::function<void()> const& drawHandler) override;

	std::unique_ptr<ICachedTexture> CreateEmptyTexture(bool cubemap) override;
	void SetTextureAnisotropy(float value = 1.0f) override;
	void UploadTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	void UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, size_t size, int flags, TextureMipMaps const& mipmaps = TextureMipMaps()) override;
	void UploadCubemap(ICachedTexture & texture, TextureMipMaps const& sides, unsigned short bpp, int flags) override;
	bool Force32Bits() const override;
	bool ForceFlipBMP() const override;
	bool ConvertBgra() const override;
	bool SupportsFeature(Feature feature) const override;
	std::string GetName() const override;

	void OnResize(unsigned int width, unsigned int height);
	ID3D11DeviceContext * GetContext();
	void SetInputLayout(DXGI_FORMAT vertexFormat, DXGI_FORMAT texCoordFormat, DXGI_FORMAT normalFormat);
	void Present();
	void ToggleFullscreen();
	void EnableMultisampling(bool enable, int level = 1.0f);
	void SetSharedIndexBuffer(CComPtr<ID3D11Buffer> const& buffer);
private:
	void MakeSureBufferCanFitSize(size_t size);
	void CreateBuffer(ID3D11Buffer ** bufferPtr, unsigned int elementSize);
	void CreateTexture(unsigned int width, unsigned int height, int flags, const void * data, ID3D11Texture2D ** texture, ID3D11ShaderResourceView ** resourceView, 
		size_t size = 0, CachedTextureType type = CachedTextureType::RGBA, TextureMipMaps const& mipmaps = TextureMipMaps(), bool cubemap = false);
	void UpdateMatrices();
	void CopyDataToBuffer(ID3D11Buffer * buffer, const void* data, size_t size);
	void CreateDepthBuffer(unsigned int width, unsigned int height, ID3D11DepthStencilView ** buffer);
	void ResetViewMatrix();

	CComPtr<IDXGISwapChain> m_swapchain;// the pointer to the swap chain interface
	CComPtr<ID3D11Device> m_dev;
	CComPtr<ID3D11DeviceContext> m_devcon;
	HWND m_hWnd;
	TextureManager * m_textureManager;
	CShaderManagerDirectX m_shaderManager;
	CComPtr<ID3D11DepthStencilState> m_depthState[2];
	CComPtr<ID3D11BlendState> m_blendStates[2];

	CComPtr<ID3D11Buffer> m_vertexBuffer;
	CComPtr<ID3D11Buffer> m_sharedIndexBuffer;
	std::vector<char> m_vertexCache;
	std::unique_ptr<ICachedTexture> m_emptyTexture;

	std::vector<DirectX::XMFLOAT4X4> m_modelMatrices;
	DirectX::XMFLOAT4X4* m_modelMatrix;
	DirectX::XMFLOAT4X4 m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	float m_anisotropyLevel = 0.0f;
	bool m_matricesChanged = true;
	mutable DirectX::XMFLOAT4X4 m_modelViewMatrix;
};