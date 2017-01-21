#pragma once
#include "../view/IViewHelper.h"
#include <d3d11.h>
#include "../view/TextureManager.h"
#include <atlcomcli.h>
#include <DirectXMath.h>
#include "ShaderManagerDirectX.h"

struct sLightSource
{
	float pos[3] = { 0.0f, 0.0f, 0.0f };
	float diffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float specular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	bool enabled = 0;
};

class CDirectXRenderer : public IViewHelper
{
public:
	CDirectXRenderer(HWND hWnd = NULL);
	~CDirectXRenderer();

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
	virtual void ResetViewMatrix() override;
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
	virtual IShaderManager& GetShaderManager() override;

	virtual void WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const override;
	virtual void WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const override;
	virtual std::unique_ptr<IFrameBuffer> CreateFramebuffer() const override;
	virtual void EnableLight(size_t index, bool enable) override;
	virtual void SetLightColor(size_t index, LightningType type, float * values) override;
	virtual void SetLightPosition(size_t index, float* pos) override;
	virtual float GetMaximumAnisotropyLevel() const override;
	virtual void GetProjectionMatrix(float * matrix) const override;
	virtual void EnableDepthTest(bool enable) override;
	virtual void EnableBlending(bool enable) override;
	virtual void SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane = 1.0f, float farPane = 1000.0f) override;
	virtual void EnablePolygonOffset(bool enable, float factor = 0.0f, float units = 0.0f) override;
	virtual void ClearBuffers(bool color = true, bool depth = true) override;
	virtual void SetTextureManager(CTextureManager & textureManager) override;
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
	virtual bool SupportsFeature(Feature feature) const override;
	virtual std::string GetName() const override;

	void SetTextureResource(ID3D11ShaderResourceView * view);
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
		bool renderTarget = false, size_t size = 0, CachedTextureType type = CachedTextureType::RGBA, TextureMipMaps const& mipmaps = TextureMipMaps());
	void UpdateMatrices();
	void CopyDataToBuffer(ID3D11Buffer * buffer, const void* data, size_t size);
	void CreateDepthBuffer(unsigned int width, unsigned int height, ID3D11DepthStencilView ** buffer);

	CComPtr<IDXGISwapChain> m_swapchain;// the pointer to the swap chain interface
	CComPtr<ID3D11Device> m_dev;
	CComPtr<ID3D11DeviceContext> m_devcon;
	HWND m_hWnd;
	CTextureManager * m_textureManager;
	CShaderManagerDirectX m_shaderManager;
	unsigned int m_activeTextureSlot;
	CComPtr<ID3D11DepthStencilState> m_depthState[2];
	CComPtr<ID3D11BlendState> m_blendStates[2];

	CComPtr<ID3D11Buffer> m_vertexBuffer;
	CComPtr<ID3D11Buffer> m_normalsBuffer;
	CComPtr<ID3D11Buffer> m_texCoordBuffer;
	CComPtr<ID3D11Buffer> m_sharedIndexBuffer;
	size_t m_buffersSize = 0;

	std::vector<sLightSource> m_lightSources;
	std::vector<DirectX::XMFLOAT4X4> m_viewMatrices;
	DirectX::XMFLOAT4X4* m_viewMatrix;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	float m_anisotropyLevel = 0.0f;
};