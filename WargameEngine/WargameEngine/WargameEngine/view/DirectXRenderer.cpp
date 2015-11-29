#include "DirectXRenderer.h"
#include <Windows.h>
#include "ShaderManagerDirectX.h"
#include "..\LogWriter.h"
#include <DirectXMath.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace DirectX;

class CDirectXCachedTexture : public ICachedTexture
{
public:
	CDirectXCachedTexture(CDirectXRenderer *renderer)
		: m_renderer(renderer), m_resourceView(nullptr), m_texture(nullptr)
	{
	}

	~CDirectXCachedTexture()
	{
		if(m_resourceView) m_resourceView->Release();
		if(m_texture) m_texture->Release();
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
	ID3D11ShaderResourceView* m_resourceView;
	ID3D11Texture2D* m_texture;
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
		:m_dev(dev), m_renderer(renderer), m_indexBufferSize(0)
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

	virtual void DrawIndexes(unsigned int * indexPtr, size_t count) override
	{		
		if (count == 0) return;
		if (count > m_indexBufferSize)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
			bd.ByteWidth = sizeof(unsigned int) * count;             // size is the index
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;       // use as a index buffer
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

			m_pIndexBuffer = nullptr;
			HRESULT hr = m_dev->CreateBuffer(&bd, NULL, &m_pIndexBuffer);       // create the buffer
			if (FAILED(hr))
			{
				LogWriter::WriteLine("DirectX error: Cannot create render target view for texture rendering");
			}
			m_indexBufferSize = count;
			Bind();
		}

		D3D11_MAPPED_SUBRESOURCE ms;
		m_renderer->GetContext()->Map(m_pIndexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
		memcpy(ms.pData, indexPtr, sizeof(unsigned int) * count);                // copy the data
		m_renderer->GetContext()->Unmap(m_pIndexBuffer, NULL);                        // unmap the buffer

		m_renderer->GetContext()->DrawIndexed(count, 0, 0);
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
private:
	CComPtr<ID3D11Buffer> m_pVertexBuffer;
	CComPtr<ID3D11Buffer> m_pTexCoordBuffer;
	CComPtr<ID3D11Buffer> m_pNormalBuffer;
	CComPtr<ID3D11Buffer> m_pIndexBuffer;
	CDirectXRenderer * m_renderer;
	CComPtr<ID3D11Device> m_dev;
	size_t m_indexBufferSize;
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
		auto& dxTexture = dynamic_cast<CDirectXCachedTexture&>(texture);
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


float GetAspectRatio(HWND m_hWnd)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return static_cast<float>(rect.right - rect.left) / static_cast<float>(rect.bottom - rect.top);
}

CDirectXRenderer::CDirectXRenderer(ID3D11Device *dev, ID3D11DeviceContext *devcon, HWND hWnd)
	:m_dev(dev), m_devcon(devcon), m_hWnd(hWnd)
	, m_defaultShaderManager(std::make_unique<CShaderManagerDirectX>(dev, this)), m_textureManager(*this), m_activeTextureSlot(0)
{
	m_viewMatrices.push_back(DirectX::XMMatrixIdentity());
	float aspect = GetAspectRatio(m_hWnd);
	m_projectionMatrices.push_back(DirectX::XMMatrixPerspectiveFovLH(1.05f, aspect, 0.05f, 1000.0f));

	m_defaultShaderManager->BindProgram();

	CreateBuffer(&m_vertexBuffer, sizeof(CVector3f) * 10000);
	CreateBuffer(&m_normalsBuffer, sizeof(CVector3f) * 10000);
	CreateBuffer(&m_texCoordBuffer, sizeof(CVector2f) * 10000);

	SetTextureAnisotropy(1.0f);
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

	CopyDataToBuffer(m_vertexBuffer, floatVertices.data(), floatVertices.size() * sizeof(float));
	CopyDataToBuffer(m_texCoordBuffer, texCoords.data(), texCoords.size() * sizeof(CVector2f));

	m_shaderManager->SetInputLayout(DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT);
	m_devcon->IASetVertexBuffers(0, 3, buffers, stride, offset);
	m_devcon->IASetPrimitiveTopology(renderModeMap.at(mode));
	m_devcon->Draw(vertices.size(), 0);
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords)
{
	UINT stride[] = { sizeof(CVector2f), sizeof(CVector2f), 0 };
	UINT offset[] = { 0, 0, 0 };
	ID3D11Buffer* buffers[] = { m_vertexBuffer, m_texCoordBuffer, nullptr };

	if (mode == RenderMode::RECTANGLES)
	{
		FlipRectangleTopology(const_cast<float*>(&vertices[0].x), !texCoords.empty() ? const_cast<float*>(&texCoords[0].x) : NULL, NULL, 2, vertices.size());
	}

	CopyDataToBuffer(m_vertexBuffer, vertices.data(), vertices.size() * sizeof(CVector2f));
	CopyDataToBuffer(m_texCoordBuffer, texCoords.data(), texCoords.size() * sizeof(CVector2f));

	m_shaderManager->SetInputLayout(DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT);
	m_devcon->IASetVertexBuffers(0, 3, buffers, stride, offset);
	m_devcon->IASetPrimitiveTopology(renderModeMap.at(mode));
	m_devcon->Draw(vertices.size(), 0);
}

std::vector<float> ConvertVector3D(std::vector<CVector3d> const& vec)
{
	std::vector<float> result;
	result.reserve(vec.size() * 3);
	for (auto& item : vec)
	{
		result.push_back(static_cast<float>(item.x));
		result.push_back(static_cast<float>(item.y));
		result.push_back(static_cast<float>(item.z));
	}
	return std::move(result);
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords)
{
	UINT stride[] = { sizeof(CVector3f), sizeof(CVector2f), sizeof(CVector3f) };
	UINT offset[] = { 0, 0, 0 };

	auto floatVertices = ConvertVector3D(vertices);
	auto floatNormals = ConvertVector3D(normals);
	std::vector<float> floatTexCoords;
	floatTexCoords.reserve(texCoords.size() * 3);
	for (auto& item : texCoords)
	{
		floatTexCoords.push_back(static_cast<float>(item.x));
		floatTexCoords.push_back(static_cast<float>(item.y));
	}

	CopyDataToBuffer(m_vertexBuffer, floatVertices.data(), floatVertices.size() * sizeof(float));
	CopyDataToBuffer(m_texCoordBuffer, floatTexCoords.data(), floatTexCoords.size() * sizeof(float));
	CopyDataToBuffer(m_normalsBuffer, floatNormals.data(), floatNormals.size() * sizeof(float));

	ID3D11Buffer* buffers[] = { m_vertexBuffer, m_texCoordBuffer, m_normalsBuffer };

	m_shaderManager->SetInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT);
	m_devcon->IASetVertexBuffers(0, 3, buffers, stride, offset);
	m_devcon->IASetPrimitiveTopology(renderModeMap.at(mode));
	m_devcon->Draw(vertices.size(), 0);
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	UINT stride[] = { sizeof(CVector3f), sizeof(CVector2f), sizeof(CVector3f) };
	UINT offset[] = { 0, 0, 0 };
	ID3D11Buffer* buffers[] = { m_vertexBuffer, m_texCoordBuffer, m_normalsBuffer };

	CopyDataToBuffer(m_vertexBuffer, vertices.data(), vertices.size() * sizeof(CVector3f));
	CopyDataToBuffer(m_texCoordBuffer, texCoords.data(), texCoords.size() * sizeof(CVector2f));
	CopyDataToBuffer(m_normalsBuffer, normals.data(), normals.size() * sizeof(CVector3f));

	m_shaderManager->SetInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT);
	m_devcon->IASetVertexBuffers(0, 3, buffers, stride, offset);
	m_devcon->IASetPrimitiveTopology(renderModeMap.at(mode));
	m_devcon->Draw(vertices.size(), 0);
}

void CDirectXRenderer::SetColor(const int * color)
{
	float fcolor[4] = { static_cast<float>(INT_MAX) / color[0], static_cast<float>(INT_MAX) / color[1], static_cast<float>(INT_MAX) / color[2], 1.0f };
	m_shaderManager->SetColor(fcolor);
}

void CDirectXRenderer::SetColor(const float * color)
{
	m_shaderManager->SetColor(color);
}

void CDirectXRenderer::SetColor(const int r, const int g, const int b)
{
	float fcolor[4] = { static_cast<float>(INT_MAX) / r, static_cast<float>(INT_MAX) / g, static_cast<float>(INT_MAX) / b, 1.0f };
	m_shaderManager->SetColor(fcolor);
}

void CDirectXRenderer::SetColor(const float r, const float g, const float b)
{
	float fcolor[4] = { r, g, b, 1.0f };
	m_shaderManager->SetColor(fcolor);
}

void CDirectXRenderer::PushMatrix()
{
	m_viewMatrices.push_back(m_viewMatrices.back());
}

void CDirectXRenderer::PopMatrix()
{
	m_viewMatrices.pop_back();
	UpdateMatrices();
}

void CDirectXRenderer::Translate(const int dx, const int dy, const int dz)
{
	auto matrix = m_viewMatrices.back();
	matrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz)), matrix);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::Translate(const double dx, const double dy, const double dz)
{
	auto matrix = m_viewMatrices.back();
	matrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz)), matrix);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::Translate(const float dx, const float dy, const float dz)
{
	auto matrix = m_viewMatrices.back();
	matrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslation(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz)), matrix);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::Rotate(const double angle, const double x, const double y, const double z)
{
	XMVECTOR axis = DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), 1.0f);
	auto matrix = m_viewMatrices.back();
	matrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationAxis(axis, static_cast<float>(angle * M_PI / 180.0)), matrix);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::Scale(const double scale)
{
	auto matrix = m_viewMatrices.back();
	float fscale = static_cast<float>(scale);
	matrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(fscale, fscale, fscale), matrix);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::GetViewMatrix(float * matrix) const
{
	XMFLOAT4X4 view;
	auto mat = m_viewMatrices.back();
	XMStoreFloat4x4(&view, mat);
	memcpy(matrix, *view.m, sizeof(view));
}

void CDirectXRenderer::ResetViewMatrix()
{
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(DirectX::XMMatrixIdentity());
	UpdateMatrices();
}

XMVECTOR Vec3ToXMVector(CVector3d const& vec)
{
	return  DirectX::XMVectorSet(-static_cast<float>(vec.x), static_cast<float>(vec.y), static_cast<float>(vec.z), static_cast<float>(vec.GetLength()));
}

void CDirectXRenderer::LookAt(CVector3d const& position, CVector3d const& direction, CVector3d const& up)
{
	XMVECTOR pos = Vec3ToXMVector(position);
	XMVECTOR dir = Vec3ToXMVector(direction);
	XMVECTOR upVec = DirectX::XMVectorSet(static_cast<float>(up.x), static_cast<float>(up.y), static_cast<float>(up.z), 1.0f);
	auto old = m_viewMatrices.back();
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(DirectX::XMMatrixMultiply(old, DirectX::XMMatrixLookAtLH(pos, dir, upVec)));
	UpdateMatrices();
}

void CDirectXRenderer::SetTexture(std::string const& texture, const std::vector<sTeamColor> * teamcolor, int flags /*= 0*/)
{
	m_textureManager.SetTexture(texture, teamcolor, flags);
}

void CDirectXRenderer::SetTexture(std::string const& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager.SetTexture(texture, slot, flags);
}

void CDirectXRenderer::SetTexture(std::string const& texture, bool forceLoadNow /*= false*/, int flags /*= 0*/)
{
	if (forceLoadNow)
	{
		m_textureManager.LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager.SetTexture(texture, nullptr, flags);
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height)
{
	CComPtr<ID3D11RenderTargetView> oldRenderTargetView;
	CComPtr<ID3D11DepthStencilView> oldDepthStencilView;
	unsigned int numViewports = 1;
	D3D11_VIEWPORT oldViewport;
	m_devcon->OMGetRenderTargets(1, &oldRenderTargetView, &oldDepthStencilView);
	m_devcon->RSGetViewports(&numViewports, &oldViewport);
	m_projectionMatrices.push_back(DirectX::XMMatrixOrthographicOffCenterLH(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 0.0f, 1.0f));
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
	m_projectionMatrices.pop_back();
	PopMatrix();

	return std::move(tex);
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type /*= CachedTextureType::RGBA*/)
{
	auto tex = std::make_unique<CDirectXCachedTexture>(this);
	CreateTexture(width, height, 0, data, &tex->m_texture, &tex->m_resourceView, type != CachedTextureType::ALPHA, 0, type);
	return std::move(tex);
}

void CDirectXRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
	m_shaderManager->SetMaterial(ambient, diffuse, specular, shininess);
}

std::unique_ptr<IDrawingList> CDirectXRenderer::CreateDrawingList(std::function<void() > const& func)
{
	return std::make_unique<CDirectXDrawingList>(func);
}

std::unique_ptr<IVertexBuffer> CDirectXRenderer::CreateVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/, size_t size)
{
	auto buffer = std::make_unique<CDirectXVertexBuffer>(m_dev, this);

	if (vertex)
	{
		CreateBuffer(buffer->GetVertexBufferPtr(), sizeof(float) * size);
		CopyDataToBuffer(buffer->GetVertexBuffer(), vertex, sizeof(float) * size);
	}
	if (normals)
	{
		CreateBuffer(buffer->GetNormalBufferPtr(), sizeof(float) * size);
		CopyDataToBuffer(buffer->GetNormalBuffer(), normals, sizeof(float) * size);
	}
	if (texcoords)
	{
		CreateBuffer(buffer->GetTexCoordBufferPtr(), sizeof(float) * size * 2 / 3);
		CopyDataToBuffer(buffer->GetTexCoordBuffer(), texcoords, sizeof(float) * size * 2 / 3);
	}

	return std::move(buffer);
}

std::unique_ptr<IShaderManager> CDirectXRenderer::CreateShaderManager() const
{
	return std::make_unique<CShaderManagerDirectX>(m_dev, const_cast<CDirectXRenderer *>(this));
}

void CDirectXRenderer::WindowCoordsToWorldVector(int x, int y, CVector3d & start, CVector3d & end) const
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	auto projection = m_projectionMatrices.back();
	auto view = m_viewMatrices.back();
	XMVECTOR vec1 = DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), 0.05f, 1.0f);
	vec1 = DirectX::XMVector3Unproject(vec1, static_cast<float>(rect.left), static_cast<float>(rect.top), static_cast<float>(rect.right),
		static_cast<float>(rect.bottom), 0.05f, 1000.0f, projection, view, DirectX::XMMatrixIdentity());
	start = CVector3d(DirectX::XMVectorGetX(vec1), DirectX::XMVectorGetY(vec1), DirectX::XMVectorGetZ(vec1));
	XMVECTOR vec2 = DirectX::XMVectorSet(static_cast<float>(x), static_cast<float>(y), 1000.0f, 1.0f);
	vec2 = DirectX::XMVector3Unproject(vec2, static_cast<float>(rect.left), static_cast<float>(rect.top), static_cast<float>(rect.right),
		static_cast<float>(rect.bottom), 0.05f, 1000.0f, projection, view, DirectX::XMMatrixIdentity());
	end = CVector3d(DirectX::XMVectorGetX(vec2), DirectX::XMVectorGetY(vec2), DirectX::XMVectorGetZ(vec2));
}

void CDirectXRenderer::WorldCoordsToWindowCoords(CVector3d const& worldCoords, int& x, int& y) const
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	XMVECTOR vec = Vec3ToXMVector(worldCoords);
	auto projection = m_projectionMatrices.back();
	auto view = m_viewMatrices.back();
	vec = DirectX::XMVector3Project(vec, static_cast<float>(rect.left), static_cast<float>(rect.top), static_cast<float>(rect.right), static_cast<float>(rect.bottom),
		0.05f, 1000.0f, projection, view, DirectX::XMMatrixIdentity());
	x = static_cast<int>(DirectX::XMVectorGetX(vec));
	y = static_cast<int>(DirectX::XMVectorGetY(vec));
}

std::unique_ptr<IFrameBuffer> CDirectXRenderer::CreateFramebuffer() const
{
	return std::make_unique<CDirectXFrameBuffer>(m_dev, const_cast<CDirectXRenderer*>(this));
}

void CDirectXRenderer::EnableLight(size_t index, bool enable)
{
	if (m_lightSources.size() <= index)
	{
		m_lightSources.resize(index + 1);
	}
	m_lightSources[index].enabled = enable;
	m_shaderManager->SetLight(index, m_lightSources[index]);
}

void CDirectXRenderer::SetLightColor(size_t index, LightningType type, float * values)
{
	if (m_lightSources.size() <= index)
	{
		m_lightSources.resize(index + 1);
	}
	float * dest = m_lightSources[index].ambient;
	if (type == LightningType::DIFFUSE) dest = m_lightSources[index].diffuse;
	if (type == LightningType::SPECULAR) dest = m_lightSources[index].specular;
	memcpy(dest, values, sizeof(float) * 3);
	m_shaderManager->SetLight(index, m_lightSources[index]);
}

void CDirectXRenderer::SetLightPosition(size_t index, float* pos)
{
	if (m_lightSources.size() <= index)
	{
		m_lightSources.resize(index + 1);
	}
	memcpy(m_lightSources[index].pos, pos, sizeof(float) * 3);
	m_shaderManager->SetLight(index, m_lightSources[index]);
}

float CDirectXRenderer::GetMaximumAnisotropyLevel() const
{
	return D3D11_REQ_MAXANISOTROPY;
}

void CDirectXRenderer::EnableVertexLightning(bool enable)
{
	if (enable)
	{
		LogWriter::WriteLine("Vertex lightning is not supported in DirectX11");
	}
}

void CDirectXRenderer::GetProjectionMatrix(float * matrix) const
{
	XMFLOAT4X4 projection;
	auto mprojection = m_projectionMatrices.back();
	XMStoreFloat4x4(&projection, mprojection);
	memcpy(matrix, *projection.m, sizeof(projection));
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

void CDirectXRenderer::SetUpViewport(CVector3d const& position, CVector3d const& target, unsigned int viewportWidth, unsigned int viewportHeight, double viewingAngle, double nearPane /*= 1.0*/, double farPane /*= 1000.0*/)
{
	m_projectionMatrices.push_back(DirectX::XMMatrixPerspectiveFovLH(static_cast<float>(viewingAngle), static_cast<float>(viewportWidth) / viewportHeight, static_cast<float>(nearPane), static_cast<float>(farPane)));
	LookAt(position, target, { 0.0, 0.0, 1.0 });
	UpdateMatrices();
}

void CDirectXRenderer::RestoreViewport()
{
	m_projectionMatrices.pop_back();
	UpdateMatrices();
}

void CDirectXRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	LogWriter::WriteLine("Polygon offset is not yet supported");
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

CTextureManager& CDirectXRenderer::GetTextureManager()
{
	return m_textureManager;
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
	auto& dxtexture = dynamic_cast<CDirectXCachedTexture&>(texture);
	CreateTexture(width, height, flags, data, &dxtexture.m_texture, &dxtexture.m_resourceView, false, 0, CachedTextureType::RGBA, mipmaps);

}

void CDirectXRenderer::UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, size_t size, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
	auto& dxtexture = dynamic_cast<CDirectXCachedTexture&>(texture);
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

std::string CDirectXRenderer::GetName() const
{
	return "DirectX11";
}

void CDirectXRenderer::SetShaderManager(CShaderManagerDirectX * shaderManager)
{
	m_shaderManager = shaderManager ? shaderManager : m_defaultShaderManager.get();
	UpdateMatrices();
}

void CDirectXRenderer::UpdateMatrices()
{
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	auto viewMatrix = m_viewMatrices.back();
	XMStoreFloat4x4(&view, viewMatrix);
	auto projectionMatrix = m_projectionMatrices.back();
	XMStoreFloat4x4(&projection, projectionMatrix);
	m_shaderManager->SetMatrices(*view.m, *projection.m);
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
	m_shaderManager->SetInputLayout(vertexFormat, texCoordFormat, normalFormat);
}

void CDirectXRenderer::SetUpViewport2D()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	m_projectionMatrices.push_back(DirectX::XMMatrixOrthographicOffCenterLH(static_cast<float>(rect.left), static_cast<float>(rect.right), 
		static_cast<float>(rect.bottom), static_cast<float>(rect.top), 0.0f, 1.0f));
	UpdateMatrices();
}

void CDirectXRenderer::SetTextureResource(ID3D11ShaderResourceView * view)
{
	ID3D11ShaderResourceView* views[] = { view };
	m_devcon->PSSetShaderResources(m_activeTextureSlot, 1, views);
	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (view) m_shaderManager->SetColor(color);
}

void CDirectXRenderer::OnResize()
{
	float aspect = GetAspectRatio(m_hWnd);
	m_projectionMatrices.pop_back();
	m_projectionMatrices.push_back(DirectX::XMMatrixPerspectiveFovLH(1.05f, aspect, 0.05f, 1000.0f));
	UpdateMatrices();
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