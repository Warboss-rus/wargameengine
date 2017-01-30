#include "DirectXRenderer.h"
#include <Windows.h>
#include "ShaderManagerDirectX.h"
#include "..\LogWriter.h"
#include <DirectXMath.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "../view/IViewport.h"

using namespace DirectX;

inline DirectX::XMFLOAT4X4 Store(DirectX::XMMATRIX const& matrix)
{
	DirectX::XMFLOAT4X4 result;
	XMStoreFloat4x4(&result, matrix);
	return result;
}

inline DirectX::XMMATRIX Load(DirectX::XMFLOAT4X4 const& matrix)
{
	return XMLoadFloat4x4(&matrix);
}

class CDirectXCachedTexture : public ICachedTexture
{
public:
	CDirectXCachedTexture(CDirectXRenderer *renderer)
		: m_renderer(renderer)
	{
	}

	virtual void Bind() const override
	{
		m_renderer->SetTextureResource(m_resourceView);
	}

	virtual void UnBind() const override
	{
		m_renderer->SetTextureResource(NULL);
	}
private:
	friend class CDirectXRenderer;
	friend class CDirectXFrameBuffer;
	CComPtr<ID3D11ShaderResourceView> m_resourceView;
	CComPtr<ID3D11Texture2D> m_texture;
	CDirectXRenderer * m_renderer;
};

class CDirectXDrawingList : public IDrawingList
{
public:
	CDirectXDrawingList(std::function<void()> const& func)
		:m_func(func)
	{}
	virtual void Draw() const override
	{
		m_func();
	}
private:
	std::function<void()> m_func;
};

class CDirectXVertexBuffer : public IVertexBuffer
{
public:
	CDirectXVertexBuffer(CComPtr<ID3D11Device> dev, CDirectXRenderer * renderer)
		:m_dev(dev), m_renderer(renderer)
	{
	}

	virtual void Bind() const override
	{
		UINT stride[] = { sizeof(float) * 3, sizeof(float) * 2, sizeof(float) * 3 };
		UINT offset[] = { 0, 0, 0 };
		ID3D11Buffer* buffers[] = { m_pVertexBuffer, m_pTexCoordBuffer, m_pNormalBuffer };
		auto context = m_renderer->GetContext();
		context->IASetVertexBuffers(0, 3, buffers, stride, offset);
		context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->SetInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT);
	}

	virtual void SetIndexBuffer(unsigned int * indexPtr, size_t indexesSize)
	{
		if (indexesSize == 0) return;
		if (indexesSize > m_indexBufferSize || !m_pIndexBuffer)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
			bd.ByteWidth = sizeof(unsigned int) * indexesSize;             // size is the index
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;       // use as a index buffer
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

			m_pIndexBuffer = nullptr;
			HRESULT hr = m_dev->CreateBuffer(&bd, NULL, &m_pIndexBuffer);       // create the buffer
			if (FAILED(hr))
			{
				LogWriter::WriteLine("DirectX error: Cannot create index buffer");
			}
			m_indexBufferSize = indexesSize;
			if (m_sharedIndexBuffer)
			{
				m_renderer->SetSharedIndexBuffer(m_pIndexBuffer);
			}
		}
		D3D11_MAPPED_SUBRESOURCE ms;
		m_renderer->GetContext()->Map(m_pIndexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
		memcpy(ms.pData, indexPtr, sizeof(unsigned int) * indexesSize);                // copy the data
		m_renderer->GetContext()->Unmap(m_pIndexBuffer, NULL);                        // unmap the buffer
	}

	virtual void DrawIndexes(size_t begin, size_t count) override
	{		
		m_renderer->GetContext()->DrawIndexed(count, begin, 0);
	}

	virtual void DrawAll(size_t count) override
	{
		m_renderer->GetContext()->Draw(0, count);
	}

	virtual void UnBind() const override
	{
		m_renderer->GetContext()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
		m_renderer->GetContext()->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	}

	ID3D11Buffer ** GetVertexBufferPtr()
	{
		return &m_pVertexBuffer;
	}

	ID3D11Buffer ** GetNormalBufferPtr()
	{
		return &m_pNormalBuffer;
	}

	ID3D11Buffer ** GetTexCoordBufferPtr()
	{
		return &m_pTexCoordBuffer;
	}

	ID3D11Buffer * GetVertexBuffer()
	{
		return m_pVertexBuffer;
	}

	ID3D11Buffer * GetNormalBuffer()
	{
		return m_pNormalBuffer;
	}

	ID3D11Buffer * GetTexCoordBuffer()
	{
		return m_pTexCoordBuffer;
	}

	void SetIndexBufferPtr(ID3D11Buffer* indexBuffer)
	{
		D3D11_BUFFER_DESC desc;
		if (indexBuffer)
		{
			indexBuffer->GetDesc(&desc);
			m_indexBufferSize = desc.ByteWidth / sizeof(unsigned int);
		}
		m_pIndexBuffer = indexBuffer;
		m_sharedIndexBuffer = true;
	}

	virtual void DrawInstanced(size_t size, size_t instanceCount) override
	{
		m_renderer->GetContext()->DrawInstanced(size, instanceCount, 0, 0);
	}

private:
	CComPtr<ID3D11Buffer> m_pVertexBuffer;
	CComPtr<ID3D11Buffer> m_pTexCoordBuffer;
	CComPtr<ID3D11Buffer> m_pNormalBuffer;
	CComPtr<ID3D11Buffer> m_pIndexBuffer;
	CDirectXRenderer * m_renderer;
	ID3D11Device* m_dev;
	size_t m_indexBufferSize = 0;
	bool m_sharedIndexBuffer = false;
};

class CDirectXFrameBuffer : public IFrameBuffer
{
public:
	CDirectXFrameBuffer(CComPtr<ID3D11Device> dev, CDirectXRenderer * renderer)
		:m_dev(dev), m_renderer(renderer)
	{

	}
	virtual void Bind() const override
	{
		m_oldDepthStencilView = nullptr;
		m_oldRenderTargetView = nullptr;
		m_renderer->GetContext()->OMGetRenderTargets(1, &m_oldRenderTargetView, &m_oldDepthStencilView);
		m_renderer->GetContext()->OMSetRenderTargets(1, &m_renderTargetView.p, m_depthStencilView);
	}

	virtual void UnBind() const override
	{
		m_renderer->GetContext()->OMSetRenderTargets(1, &m_oldRenderTargetView.p, m_oldDepthStencilView);
		m_oldRenderTargetView = nullptr;
		m_oldRenderTargetView = nullptr;
	}

	virtual void AssignTexture(ICachedTexture & texture, CachedTextureType type) override
	{
		auto& dxTexture = reinterpret_cast<CDirectXCachedTexture&>(texture);
		if (type == CachedTextureType::DEPTH)
		{
			m_depthStencilView = nullptr;

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			m_dev->CreateDepthStencilView(dxTexture.m_texture, &depthStencilViewDesc, &m_depthStencilView);
		}
		else
		{
			m_renderTargetView = nullptr;

			D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
			renderTargetViewDesc.Format = type == CachedTextureType::RGBA ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_A8_UNORM;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			m_dev->CreateRenderTargetView(dxTexture.m_texture, &renderTargetViewDesc, &m_renderTargetView);
		}
	}
private:
	CDirectXRenderer * m_renderer;
	CComPtr<ID3D11Device> m_dev;
	mutable CComPtr<ID3D11RenderTargetView> m_oldRenderTargetView;
	mutable CComPtr<ID3D11DepthStencilView> m_oldDepthStencilView;
	CComPtr<ID3D11RenderTargetView> m_renderTargetView;
	CComPtr<ID3D11DepthStencilView> m_depthStencilView;
};

class CDirectXOcclusionQuery : public IOcclusionQuery
{
public:
	CDirectXOcclusionQuery(ID3D11Device * dev, CDirectXRenderer * renderer)
		: m_renderer(renderer)
	{
		D3D11_QUERY_DESC desc;
		desc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
		desc.MiscFlags = 0;
		CComPtr<ID3D11Query> query;
		dev->CreateQuery(&desc, &m_query);
	}

	virtual void Query(std::function<void() > const& handler, bool renderToScreen) override
	{
		m_renderer->GetContext()->Begin(m_query);
		handler();
		m_renderer->GetContext()->End(m_query);
	}

	virtual bool IsVisible() const override
	{
		BOOL result = 1;
		m_renderer->GetContext()->GetData(m_query, &result, sizeof(BOOL), 0);
		return result != FALSE;
	}
private:
	CComPtr<ID3D11Query> m_query;
	CDirectXRenderer * m_renderer;
};

float GetAspectRatio(HWND m_hWnd)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return static_cast<float>(rect.right - rect.left) / static_cast<float>(rect.bottom - rect.top);
}

CDirectXRenderer::CDirectXRenderer(HWND hWnd)
	:m_hWnd(hWnd)
	, m_textureManager(nullptr)
	, m_activeTextureSlot(0)
	, m_shaderManager(this)
{
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = m_hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
//#ifdef _DEBUG
//		D3D11_CREATE_DEVICE_DEBUG,
//#else
		0,
//#endif
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_swapchain,
		&m_dev,
		NULL,
		&m_devcon);

	if (FAILED(hr))
	{
		LogWriter::WriteLine("DirectX error: Cannot create Swapchain");
	}
	m_shaderManager.SetDevice(m_dev);
	m_shaderManager.DoOnProgramChange([this] {
		UpdateMatrices();
	});

	// get the address of the back buffer
	CComPtr<ID3D11Texture2D> pBackBuffer;
	m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	CComPtr<ID3D11RenderTargetView> backBuffer;
	hr = m_dev->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
	if (FAILED(hr))
	{
		LogWriter::WriteLine("DirectX error: Cannot create backbuffer");
	}
	pBackBuffer = NULL;
	CComPtr<ID3D11DepthStencilView> pDepthStencilView;
	CreateDepthBuffer(600, 600, &pDepthStencilView);
	m_devcon->OMSetRenderTargets(1, &backBuffer.p, pDepthStencilView);

	backBuffer = NULL;
	pDepthStencilView = NULL;

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.Width = 600;
	viewport.Height = 600;
	viewport.MaxDepth = 1.0f;

	m_devcon->RSSetViewports(1, &viewport);

	D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerState.AntialiasedLineEnable = TRUE;
	rasterizerState.CullMode = D3D11_CULL_NONE; // D3D11_CULL_FRONT or D3D11_CULL_NONE D3D11_CULL_BACK
	rasterizerState.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_SOLID  D3D11_FILL_WIREFRAME
	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 0.0f;
	rasterizerState.DepthClipEnable = FALSE;
	rasterizerState.FrontCounterClockwise = FALSE;
	rasterizerState.MultisampleEnable = TRUE;
	rasterizerState.ScissorEnable = FALSE;
	rasterizerState.SlopeScaledDepthBias = 0.0f;

	CComPtr<ID3D11RasterizerState> pRasterState;
	hr = m_dev->CreateRasterizerState(&rasterizerState, &pRasterState);
	if (FAILED(hr))
	{
		LogWriter::WriteLine("DirectX error: Cannot create rasterizer state");
	}
	m_devcon->RSSetState(pRasterState);
	
	m_modelMatrices.push_back(Store(DirectX::XMMatrixIdentity()));
	m_modelMatrix = &m_modelMatrices.back();
	m_viewMatrix = Store(DirectX::XMMatrixIdentity());
	float aspect = GetAspectRatio(m_hWnd);
	m_projectionMatrix = Store(DirectX::XMMatrixPerspectiveFovLH(1.05f, aspect, 0.05f, 1000.0f));

	MakeSureBufferCanFitSize(10000);

	SetTextureAnisotropy(1.0f);
}

void CDirectXRenderer::MakeSureBufferCanFitSize(size_t size)
{
	if (size > m_buffersSize || !m_vertexBuffer)
	{
		m_buffersSize = size;
		m_vertexBuffer = nullptr;
		m_normalsBuffer = nullptr;
		m_texCoordBuffer = nullptr;
		CreateBuffer(&m_vertexBuffer, sizeof(CVector3f) * size);
		CreateBuffer(&m_normalsBuffer, sizeof(CVector3f) * size);
		CreateBuffer(&m_texCoordBuffer, sizeof(CVector2f) * size);
	}
}

CDirectXRenderer::~CDirectXRenderer()
{
	m_swapchain->SetFullscreenState(FALSE, NULL);
	m_devcon = NULL;
	m_dev = NULL;
	m_swapchain = NULL;

}

void CDirectXRenderer::CreateBuffer(ID3D11Buffer ** bufferPtr, unsigned int elementSize)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access by CPU and GPU
	bd.ByteWidth = elementSize;             // size is the VERTEX struct
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	HRESULT hr = m_dev->CreateBuffer(&bd, NULL, bufferPtr);       // create the buffer
	if (FAILED(hr))
	{
		LogWriter::WriteLine("DirectX error: Cannot create buffer");
	}
}

std::map<RenderMode, D3D11_PRIMITIVE_TOPOLOGY> renderModeMap = {
	{ RenderMode::LINES, D3D11_PRIMITIVE_TOPOLOGY_LINELIST },
	{ RenderMode::LINE_LOOP, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP },
	{ RenderMode::RECTANGLES, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
	{ RenderMode::TRIANGLES, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
	{ RenderMode::TRIANGLE_STRIP, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
};

void FlipRectangleTopology(float * vertices, float * texcoords, float * normals, size_t stride, size_t count)
{
	float temp[3];
	size_t index;
	for (size_t i = 0; i < count ; i += 4)
	{
		index = (i + 2) * stride;
		if (vertices)
		{
			memcpy(temp, vertices + index, sizeof(float) * stride);
			memcpy(vertices + index, vertices + index + stride, sizeof(float) * stride);
			memcpy(vertices + index + stride, temp, sizeof(float) * stride);

		}
		if (normals)
		{
			memcpy(temp, normals + index, sizeof(float) * stride);
			memcpy(normals + index, normals + index + stride, sizeof(float) * stride);
			memcpy(normals + index + stride, temp, sizeof(float) * stride);
		}
		if (texcoords)
		{
			memcpy(temp, vertices + index, sizeof(float) * 2);
			memcpy(vertices + index, vertices + index + 2, sizeof(float) * 2);
			memcpy(vertices + index + 2, temp, sizeof(float) * 2);
		}
	}
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
	UINT stride[] = { sizeof(CVector2f), sizeof(CVector2f), sizeof(CVector3f) };
	UINT offset[] = { 0, 0, 0 };
	ID3D11Buffer* buffers[] = { m_vertexBuffer, m_texCoordBuffer, NULL };

	std::vector<float> floatVertices;
	floatVertices.reserve(vertices.size() * 2);
	for (auto& vec : vertices)
	{
		floatVertices.push_back(static_cast<float>(vec.x));
		floatVertices.push_back(static_cast<float>(vec.y));
	}
	if (mode == RenderMode::RECTANGLES)
	{
		FlipRectangleTopology(floatVertices.data(), !texCoords.empty() ? const_cast<float*>(&texCoords[0].x) : NULL, NULL, 2, vertices.size());
	}

	MakeSureBufferCanFitSize(vertices.size());
	CopyDataToBuffer(m_vertexBuffer, floatVertices.data(), floatVertices.size() * sizeof(float));
	CopyDataToBuffer(m_texCoordBuffer, texCoords.data(), texCoords.size() * sizeof(CVector2f));

	m_shaderManager.SetInputLayout(DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT);
	m_devcon->IASetVertexBuffers(0, 3, buffers, stride, offset);
	m_devcon->IASetPrimitiveTopology(renderModeMap.at(mode));
	m_devcon->Draw(vertices.size(), 0);
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	UINT stride[] = { sizeof(CVector3f), sizeof(CVector2f), sizeof(CVector3f) };
	UINT offset[] = { 0, 0, 0 };
	ID3D11Buffer* buffers[] = { m_vertexBuffer, m_texCoordBuffer, normals.empty() ? nullptr : m_normalsBuffer };

	MakeSureBufferCanFitSize(vertices.size());
	CopyDataToBuffer(m_vertexBuffer, vertices.data(), vertices.size() * sizeof(CVector3f));
	CopyDataToBuffer(m_texCoordBuffer, texCoords.data(), texCoords.size() * sizeof(CVector2f));
	if(!normals.empty()) CopyDataToBuffer(m_normalsBuffer, normals.data(), normals.size() * sizeof(CVector3f));

	m_shaderManager.SetInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT);
	m_devcon->IASetVertexBuffers(0, 3, buffers, stride, offset);
	m_devcon->IASetPrimitiveTopology(renderModeMap.at(mode));
	m_devcon->Draw(vertices.size(), 0);
}

void CDirectXRenderer::SetColor(const int * color)
{
	float fcolor[4] = { static_cast<float>(INT_MAX) / color[0], static_cast<float>(INT_MAX) / color[1], static_cast<float>(INT_MAX) / color[2], 1.0f };
	m_shaderManager.SetColor(fcolor);
}

void CDirectXRenderer::SetColor(const float * color)
{
	m_shaderManager.SetColor(color);
}

void CDirectXRenderer::SetColor(const int r, const int g, const int b, const int a)
{
	float fcolor[4] = { static_cast<float>(INT_MAX) / r, static_cast<float>(INT_MAX) / g, static_cast<float>(INT_MAX) / b, static_cast<float>(INT_MAX) / a };
	m_shaderManager.SetColor(fcolor);
}

void CDirectXRenderer::SetColor(const float r, const float g, const float b, const float a)
{
	float fcolor[4] = { r, g, b, a };
	m_shaderManager.SetColor(fcolor);
}

void CDirectXRenderer::PushMatrix()
{
	m_modelMatrices.push_back(*m_modelMatrix);
	m_modelMatrix = &m_modelMatrices.back();
}

void CDirectXRenderer::PopMatrix()
{
	m_modelMatrices.pop_back();
	m_modelMatrix = &m_modelMatrices.back();
	UpdateMatrices();
}

void CDirectXRenderer::Translate(const int dx, const int dy, const int dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void CDirectXRenderer::Translate(const double dx, const double dy, const double dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void CDirectXRenderer::Translate(const float dx, const float dy, const float dz)
{
	if (abs(dx) < FLT_EPSILON && abs(dy) < FLT_EPSILON && abs(dz) < FLT_EPSILON) return;
	*m_modelMatrix = Store(DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(dx, dy, dz), Load(*m_modelMatrix)));
	UpdateMatrices();
}

void CDirectXRenderer::Rotate(const double angle, const double x, const double y, const double z)
{
	if (fabs(angle) < DBL_EPSILON) return;
	XMVECTOR axis = DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 1.0f);
	*m_modelMatrix = Store(DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationAxis(axis, static_cast<float>(angle * M_PI / 180.0)), Load(*m_modelMatrix)));
	UpdateMatrices();
}

void CDirectXRenderer::Scale(const double scale)
{
	if (fabs(scale - 1.0) < DBL_EPSILON) return;
	float fscale = static_cast<float>(scale);
	*m_modelMatrix = Store(DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(fscale, fscale, fscale), Load(*m_modelMatrix)));
	UpdateMatrices();
}

void CDirectXRenderer::GetViewMatrix(float * matrix) const
{
	auto m = Store(DirectX::XMMatrixMultiply(Load(*m_modelMatrix), Load(m_viewMatrix)));
	memcpy(matrix, *m.m, sizeof(float) * 16);
}

void CDirectXRenderer::ResetViewMatrix()
{
	*m_modelMatrix = Store(DirectX::XMMatrixIdentity());
	m_viewMatrix = Store(DirectX::XMMatrixIdentity());
	UpdateMatrices();
}

XMVECTOR Vec3ToXMVector(CVector3f const& vec)
{
	return DirectX::XMVectorSet(-vec.x, vec.y, vec.z, vec.GetLength());
}

void CDirectXRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	XMVECTOR pos = Vec3ToXMVector(position);
	XMVECTOR dir = Vec3ToXMVector(direction);
	XMVECTOR upVec = Vec3ToXMVector(up);
	m_viewMatrix = Store(DirectX::XMMatrixLookAtLH(pos, dir, upVec));
	*m_modelMatrix = Store(DirectX::XMMatrixIdentity());
	UpdateMatrices();
}

void CDirectXRenderer::SetTexture(std::wstring const& texture, const std::vector<sTeamColor> * teamcolor, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, teamcolor, flags);
}

void CDirectXRenderer::SetTexture(std::wstring const& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, flags);
}

void CDirectXRenderer::SetTexture(std::wstring const& texture, bool forceLoadNow /*= false*/, int flags /*= 0*/)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, nullptr, flags);
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height)
{
	CComPtr<ID3D11RenderTargetView> oldRenderTargetView;
	CComPtr<ID3D11DepthStencilView> oldDepthStencilView;
	unsigned int numViewports = 1;
	D3D11_VIEWPORT oldViewport;
	m_devcon->OMGetRenderTargets(1, &oldRenderTargetView, &oldDepthStencilView);
	m_devcon->RSGetViewports(&numViewports, &oldViewport);
	auto oldProjectionMatrix = m_projectionMatrix;
	auto oldViewMatrix = m_viewMatrix;
	m_projectionMatrix = Store(DirectX::XMMatrixOrthographicOffCenterLH(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 0.0f, 1.0f));
	PushMatrix();
	ResetViewMatrix();

	auto tex = std::make_unique<CDirectXCachedTexture>(this);
	CreateTexture(width, height, TEXTURE_HAS_ALPHA, NULL, &tex->m_texture, &tex->m_resourceView, true);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	CComPtr<ID3D11RenderTargetView> renderTargetView;
	HRESULT hr = m_dev->CreateRenderTargetView(tex->m_texture, &renderTargetViewDesc, &renderTargetView);
	if (FAILED(hr))
	{
		LogWriter::WriteLine("DirectX error: Cannot create render target view for texture rendering");
	}

	m_devcon->OMSetRenderTargets(1, &renderTargetView.p, nullptr);
	D3D11_VIEWPORT viewport = {0.0f, 0.0f, static_cast<FLOAT>(width), static_cast<FLOAT>(height), 0.0f, 1.0f};
	m_devcon->RSSetViewports(numViewports, &viewport);

	func();

	m_devcon->OMSetRenderTargets(1, &oldRenderTargetView.p, oldDepthStencilView);
	m_devcon->RSSetViewports(numViewports, &oldViewport);
	m_projectionMatrix = oldProjectionMatrix;
	m_viewMatrix = oldViewMatrix;
	PopMatrix();

	return std::move(tex);
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type /*= CachedTextureType::RGBA*/)
{
	auto tex = std::make_unique<CDirectXCachedTexture>(this);
	CreateTexture(width, height, 0, data, &tex->m_texture, &tex->m_resourceView, type != CachedTextureType::ALPHA, 0, type);
	return std::move(tex);
}

ICachedTexture* CDirectXRenderer::GetTexturePtr(std::wstring const& texture) const
{
	return m_textureManager->GetTexturePtr(texture);
}

void CDirectXRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
	m_shaderManager.SetMaterial(ambient, diffuse, specular, shininess);
}

std::unique_ptr<IDrawingList> CDirectXRenderer::CreateDrawingList(std::function<void() > const& func)
{
	return std::make_unique<CDirectXDrawingList>(func);
}

std::unique_ptr<IVertexBuffer> CDirectXRenderer::CreateVertexBuffer(const float * vertex, const float * normals, const float * texcoords, size_t size, bool temp)
{
	auto buffer = std::make_unique<CDirectXVertexBuffer>(m_dev, this);

	if(temp) MakeSureBufferCanFitSize(size);
	if (vertex)
	{
		if (temp)
			*buffer->GetVertexBufferPtr() = m_vertexBuffer;
		else
			CreateBuffer(buffer->GetVertexBufferPtr(), size * 3 * sizeof(float));
		CopyDataToBuffer(buffer->GetVertexBuffer(), vertex, sizeof(float) * size * 3);
	}
	if (normals)
	{
		if(temp)
			*buffer->GetNormalBufferPtr() = m_normalsBuffer;
		else
			CreateBuffer(buffer->GetNormalBufferPtr(), size * 3 * sizeof(float));
		CopyDataToBuffer(buffer->GetNormalBuffer(), normals, sizeof(float) * size * 3);
	}
	if (texcoords)
	{
		if(temp)
			*buffer->GetTexCoordBufferPtr() = m_texCoordBuffer;
		else
			CreateBuffer(buffer->GetTexCoordBufferPtr(), size * 2 * sizeof(float));
		CopyDataToBuffer(buffer->GetTexCoordBuffer(), texcoords, sizeof(float) * size * 2);
	}
	if (temp)
	{
		buffer->SetIndexBufferPtr(m_sharedIndexBuffer);
	}

	return std::move(buffer);
}

std::unique_ptr<IOcclusionQuery> CDirectXRenderer::CreateOcclusionQuery()
{
	return std::make_unique<CDirectXOcclusionQuery>(m_dev, this);
}

IShaderManager& CDirectXRenderer::GetShaderManager()
{
	return m_shaderManager;
}

void CDirectXRenderer::WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const
{
	DirectX::XMMATRIX projection(viewport.GetProjectionMatrix());
	DirectX::XMMATRIX view(viewport.GetViewMatrix());
	XMVECTOR vec1 = DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), 0.05f, 1.0f);
	vec1 = DirectX::XMVector3Unproject(vec1, static_cast<float>(viewport.GetX()), static_cast<float>(viewport.GetY()), 
		static_cast<float>(viewport.GetWidth()), static_cast<float>(viewport.GetHeight()), 0.05f, 1000.0f, projection, view, 
		DirectX::XMMatrixIdentity());
	start = CVector3f(DirectX::XMVectorGetX(vec1), DirectX::XMVectorGetY(vec1), DirectX::XMVectorGetZ(vec1));
	XMVECTOR vec2 = DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), 1000.0f, 1.0f);
	vec2 = DirectX::XMVector3Unproject(vec2, static_cast<float>(viewport.GetX()), static_cast<float>(viewport.GetY()), 
		static_cast<float>(viewport.GetWidth()), static_cast<float>(viewport.GetHeight()), 0.05f, 1000.0f, projection, view, 
		DirectX::XMMatrixIdentity());
	end = CVector3f(DirectX::XMVectorGetX(vec2), DirectX::XMVectorGetY(vec2), DirectX::XMVectorGetZ(vec2));
}

void CDirectXRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	XMVECTOR vec = DirectX::XMVectorSet(worldCoords.x, worldCoords.y, worldCoords.z, 1.0f);
	DirectX::XMMATRIX projection(viewport.GetProjectionMatrix());
	DirectX::XMMATRIX view(viewport.GetViewMatrix());
	vec = DirectX::XMVector3Project(vec, static_cast<float>(viewport.GetX()), static_cast<float>(viewport.GetY()), 
		static_cast<float>(viewport.GetWidth()), static_cast<float>(viewport.GetHeight()),
		0.05f, 1000.0f, projection, view, DirectX::XMMatrixIdentity());
	x = static_cast<int>(DirectX::XMVectorGetX(vec));
	y = static_cast<int>(DirectX::XMVectorGetY(vec));
}

std::unique_ptr<IFrameBuffer> CDirectXRenderer::CreateFramebuffer() const
{
	return std::make_unique<CDirectXFrameBuffer>(m_dev, const_cast<CDirectXRenderer*>(this));
}

void CDirectXRenderer::SetNumberOfLights(size_t count)
{
	static const std::string numberOfLightsKey = "lightsCount";
	int number = static_cast<int>(count);
	m_shaderManager.SetUniformValue(numberOfLightsKey, 1, 1, &number);
}

void CDirectXRenderer::SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular)
{
	sLightSource light;
	memcpy(light.ambient, ambient, sizeof(float) * 4);
	memcpy(light.diffuse, diffuse, sizeof(float) * 4);
	memcpy(light.specular, specular, sizeof(float) * 4);
	memcpy(light.pos, position.ptr(), sizeof(float) * 3);
	m_shaderManager.SetLight(index, light);
}

float CDirectXRenderer::GetMaximumAnisotropyLevel() const
{
	return D3D11_REQ_MAXANISOTROPY;
}

void CDirectXRenderer::GetProjectionMatrix(float * matrix) const
{
	memcpy(matrix, *m_projectionMatrix.m, sizeof(float) * 16);
}

void CDirectXRenderer::EnableDepthTest(bool enable)
{
	size_t index = enable ? 1 : 0;
	if (!m_depthState[index])
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;

		dsDesc.DepthEnable = enable ? TRUE : FALSE;
		dsDesc.DepthWriteMask = enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = enable ? D3D11_COMPARISON_LESS : D3D11_COMPARISON_ALWAYS;

		// Stencil test parameters
		dsDesc.StencilEnable = FALSE;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create depth stencil state
		HRESULT hr = m_dev->CreateDepthStencilState(&dsDesc, &m_depthState[index]);
		if (FAILED(hr))
		{
			LogWriter::WriteLine("DirectX error: Cannot create depth state");
		}
	}

	m_devcon->OMSetDepthStencilState(m_depthState[index], 0);
}

void CDirectXRenderer::EnableBlending(bool enable)
{
	size_t index = enable ? 1 : 0;
	if (!m_blendStates[index])
	{
		D3D11_BLEND_DESC descr;
		descr.AlphaToCoverageEnable = FALSE;
		descr.IndependentBlendEnable = FALSE;
		descr.RenderTarget[0].BlendEnable = enable ? TRUE : FALSE;
		descr.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		descr.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		descr.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		descr.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		descr.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
		descr.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		descr.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = m_dev->CreateBlendState(&descr, &m_blendStates[index]);
		if (FAILED(hr))
		{ 
			LogWriter::WriteLine("DirectX error: Cannot create blend state");
		}
	}
	m_devcon->OMSetBlendState(m_blendStates[index], NULL, 0xffffffff);
}

void CDirectXRenderer::SetUpViewport(unsigned int /*viewportX*/, unsigned int /*viewportY*/, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane /*= 1.0*/, float farPane /*= 1000.0*/)
{
	m_projectionMatrix = Store(DirectX::XMMatrixPerspectiveFovLH(static_cast<float>(viewingAngle * 180.0 / M_PI), static_cast<float>(viewportWidth) / viewportHeight, nearPane, farPane));
	UpdateMatrices();
}

void CDirectXRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	/*CComPtr<ID3D11RasterizerState> rasterizerState;
	m_devcon->RSGetState(&rasterizerState);
	D3D11_RASTERIZER_DESC desc;
	rasterizerState->GetDesc(&desc);
	desc.DepthBias = enable ? static_cast<INT>(units) : 0;
	desc.SlopeScaledDepthBias = enable ? factor : 1.0f;
	rasterizerState = nullptr;
	m_dev->CreateRasterizerState(&desc, &rasterizerState);
	m_devcon->RSSetState(rasterizerState);*/
}

void CDirectXRenderer::ClearBuffers(bool color /*= true*/, bool depth /*= true*/)
{
	CComPtr<ID3D11RenderTargetView> backbuffer;
	CComPtr<ID3D11DepthStencilView> depthBuffer;
	m_devcon->OMGetRenderTargets(1, &backbuffer, &depthBuffer);
	FLOAT backgroundColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if(backbuffer && color) m_devcon->ClearRenderTargetView(backbuffer, backgroundColor);
	if (depthBuffer && depth) m_devcon->ClearDepthStencilView(depthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CDirectXRenderer::SetTextureManager(CTextureManager& texMan)
{
	m_textureManager = &texMan;
}

void CDirectXRenderer::ActivateTextureSlot(TextureSlot slot)
{
	m_activeTextureSlot = static_cast<unsigned int>(slot);
}

void CDirectXRenderer::UnbindTexture()
{
	SetTextureResource(NULL);
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::CreateEmptyTexture()
{
	return std::make_unique<CDirectXCachedTexture>(this);
}

void CDirectXRenderer::SetTextureAnisotropy(float value /*= 1.0f*/)
{
	if (value == m_anisotropyLevel) return;
	m_anisotropyLevel = value;
	CComPtr<ID3D11SamplerState> state;
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Filter = D3D11_FILTER_ANISOTROPIC;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = static_cast<UINT>(value);
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
	HRESULT hr = m_dev->CreateSamplerState(&desc, &state);
	if (FAILED(hr))
	{
		LogWriter::WriteLine("DirectX error: Cannot create sampler state");
	}

	m_devcon->PSSetSamplers(0, 1, &state.p);
}

void CDirectXRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
	assert(bpp == 32);
	bpp;
	auto& dxtexture = reinterpret_cast<CDirectXCachedTexture&>(texture);
	CreateTexture(width, height, flags, data, &dxtexture.m_texture, &dxtexture.m_resourceView, false, 0, CachedTextureType::RGBA, mipmaps);

}

void CDirectXRenderer::UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, size_t size, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
	auto& dxtexture = reinterpret_cast<CDirectXCachedTexture&>(texture);
	CreateTexture(width, height, flags, data, &dxtexture.m_texture, &dxtexture.m_resourceView, false, size, CachedTextureType::RGBA, mipmaps);
}

bool CDirectXRenderer::Force32Bits() const
{
	return true;
}

bool CDirectXRenderer::ForceFlipBMP() const
{
	return true;
}

bool CDirectXRenderer::ConvertBgra() const
{
	return false;
}

std::string CDirectXRenderer::GetName() const
{
	return "DirectX11";
}

DirectX::XMFLOAT4X4 Transpose(DirectX::XMFLOAT4X4 const& m)
{
	return Store(DirectX::XMMatrixTranspose(Load(m)));
}

void CDirectXRenderer::UpdateMatrices()
{
	static const std::string mvpMatrixKey = "mvp_matrix";
	static const std::string viewMatrixKey = "view_matrix";
	static const std::string modelMatrixKey = "model_matrix";
	static const std::string projMatrixKey = "proj_matrix";
	auto mvp_matrix = Store(DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(Load(*m_modelMatrix), Load(m_viewMatrix)), Load(m_projectionMatrix))));
	m_shaderManager.SetUniformValue(mvpMatrixKey, 16, 1, *mvp_matrix.m);
	m_shaderManager.SetUniformValue(viewMatrixKey, 16, 1, *Transpose(m_viewMatrix).m);
	m_shaderManager.SetUniformValue(modelMatrixKey, 16, 1, *Transpose(*m_modelMatrix).m);
	m_shaderManager.SetUniformValue(projMatrixKey, 16, 1, *Transpose(m_projectionMatrix).m);
}

void CDirectXRenderer::CopyDataToBuffer(ID3D11Buffer * buffer, const void* data, size_t size)
{
	if (size == 0) return;
	D3D11_MAPPED_SUBRESOURCE ms;
	m_devcon->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, data, size);                // copy the data
	m_devcon->Unmap(buffer, NULL);                        // unmap the buffer
}

void CDirectXRenderer::SetInputLayout(DXGI_FORMAT vertexFormat, DXGI_FORMAT texCoordFormat, DXGI_FORMAT normalFormat)
{
	m_shaderManager.SetInputLayout(vertexFormat, texCoordFormat, normalFormat);
}

void CDirectXRenderer::Present()
{
	m_swapchain->Present(0, 0);
}

void CDirectXRenderer::ToggleFullscreen()
{
	BOOL fullscreen;
	CComPtr<IDXGIOutput> pOutput;
	m_swapchain->GetFullscreenState(&fullscreen, &pOutput);
	m_swapchain->SetFullscreenState(!fullscreen, pOutput);
}

void CDirectXRenderer::EnableMultisampling(bool enable, int level /*= 1.0f*/)
{
	DXGI_SWAP_CHAIN_DESC scd;
	m_swapchain->GetDesc(&scd);
	scd.SampleDesc.Count = enable ? static_cast<UINT>(level) : 1;
	//Recreate swap chain?
}

void CDirectXRenderer::SetSharedIndexBuffer(CComPtr<ID3D11Buffer> const& buffer)
{
	m_sharedIndexBuffer = buffer;
}

bool CDirectXRenderer::SupportsFeature(Feature) const
{
	return true;//DirectX cannot have unsupported features
}

void CDirectXRenderer::DrawIn2D(std::function<void()> const& drawHandler)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	auto oldProjectionMatrix = m_projectionMatrix;
	auto oldViewMatrix = m_viewMatrix;
	m_projectionMatrix = Store(DirectX::XMMatrixOrthographicOffCenterLH(static_cast<float>(rect.left), static_cast<float>(rect.right),
		static_cast<float>(rect.bottom), static_cast<float>(rect.top), 0.0f, 1.0f));
	PushMatrix();
	ResetViewMatrix();
	drawHandler();
	m_projectionMatrix = oldProjectionMatrix;
	m_viewMatrix = oldViewMatrix;
	PopMatrix();
}

void CDirectXRenderer::SetTextureResource(ID3D11ShaderResourceView * view)
{
	ID3D11ShaderResourceView* views[] = { view };
	m_devcon->PSSetShaderResources(m_activeTextureSlot, 1, views);
	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (view) m_shaderManager.SetColor(color);
}

void CDirectXRenderer::OnResize(unsigned int width, unsigned int height)
{
	if (m_swapchain)
	{
		m_swapchain->ResizeBuffers(1, width, height, DXGI_FORMAT_UNKNOWN, 0);
	}
	if (m_dev)
	{
		CComPtr<ID3D11Texture2D> pBackBuffer;
		m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		CComPtr<ID3D11RenderTargetView> backBuffer;
		m_dev->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
		CComPtr<ID3D11DepthStencilView> pDepthStencilView;
		CreateDepthBuffer(width, height, &pDepthStencilView);
		m_devcon->OMSetRenderTargets(1, &backBuffer.p, pDepthStencilView);
	}
	if (m_devcon)
	{
		D3D11_VIEWPORT viewport;
		unsigned int num = 1;
		m_devcon->RSGetViewports(&num, &viewport);
		viewport.Width = static_cast<FLOAT>(width);
		viewport.Height = static_cast<FLOAT>(height);
		m_devcon->RSSetViewports(num, &viewport);
	}
}

ID3D11DeviceContext * CDirectXRenderer::GetContext()
{
	return m_devcon;
}

void CDirectXRenderer::CreateTexture(unsigned int width, unsigned int height, int flags, const void * data, ID3D11Texture2D ** texture, ID3D11ShaderResourceView ** resourceView, 
	bool renderTarget, size_t size, CachedTextureType type, TextureMipMaps const& mipmaps)
{
	if (width == 0 || height == 0) return;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = mipmaps.size() + 1;
	desc.ArraySize = 1;
	DXGI_FORMAT format = flags & TEXTURE_BGRA ? DXGI_FORMAT_B8G8R8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
	if (type == CachedTextureType::DEPTH) format = DXGI_FORMAT_R32_TYPELESS;
	if (type == CachedTextureType::ALPHA) format = DXGI_FORMAT_A8_UNORM;
	if (size)
	{
		static const std::map<int, DXGI_FORMAT> compressionMap = {
			{ TEXTURE_COMPRESSION_DXT1_NO_ALPHA, DXGI_FORMAT_BC1_UNORM },
			{ TEXTURE_COMPRESSION_DXT1, DXGI_FORMAT_BC1_UNORM },
			{ TEXTURE_COMPRESSION_DXT3, DXGI_FORMAT_BC2_UNORM },
			{ TEXTURE_COMPRESSION_DXT5, DXGI_FORMAT_BC3_UNORM }
		};
		format = compressionMap.at(flags & TEXTURE_COMPRESSION_MASK);
	}
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if (renderTarget || flags & TEXTURE_BUILD_MIPMAPS) desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	if (type == CachedTextureType::DEPTH)
	{
		desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		desc.BindFlags &= ~D3D11_BIND_RENDER_TARGET;
	}
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = flags & TEXTURE_BUILD_MIPMAPS ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	int bpp = (type == CachedTextureType::ALPHA) ? 1 : 4;

	D3D11_SUBRESOURCE_DATA* texData = new D3D11_SUBRESOURCE_DATA[mipmaps.size() + 1];
	texData[0].SysMemPitch = bpp * width;
	texData[0].SysMemSlicePitch = texData[0].SysMemPitch * height;
	texData[0].pSysMem = data;
	for (size_t i = 0; i < mipmaps.size(); ++i)
	{
		texData[i + 1].pSysMem = mipmaps[i].data;
		texData[i + 1].SysMemPitch = bpp * mipmaps[i].width;
		texData[i + 1].SysMemSlicePitch = texData[i + 1].SysMemSlicePitch * mipmaps[i].height;
	}

	auto hr = m_dev->CreateTexture2D(&desc, data ? texData : nullptr, texture);
	if (FAILED(hr) || !*texture)
	{
		LogWriter::WriteLine("Cannot create texture: " + std::to_string(GetLastError()));
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC rDesc;
	ZeroMemory(&rDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	rDesc.Format = (type == CachedTextureType::DEPTH) ? DXGI_FORMAT_R32_FLOAT : format;
	rDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	rDesc.Texture2D.MostDetailedMip = 0;
	rDesc.Texture2D.MipLevels = desc.MipLevels;

	hr = m_dev->CreateShaderResourceView(*texture, &rDesc, resourceView);
	if (FAILED(hr) || !resourceView)
	{
		LogWriter::WriteLine("Cannot create resourceView: " + std::to_string(GetLastError()));
		return;
	}
}

void CDirectXRenderer::CreateDepthBuffer(unsigned int width, unsigned int height, ID3D11DepthStencilView ** buffer)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 4;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	CComPtr<ID3D11Texture2D> pDepthStencilBuffer;
	m_dev->CreateTexture2D(&depthBufferDesc, NULL, &pDepthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	m_dev->CreateDepthStencilView(pDepthStencilBuffer, &depthStencilViewDesc, buffer);
}