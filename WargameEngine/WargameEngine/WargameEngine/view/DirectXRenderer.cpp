#include "DirectXRenderer.h"
#include <Windows.h>
#include "ShaderManagerDirectX.h"
#include "..\LogWriter.h"
#include <DirectXMath.h>

using namespace DirectX;

class CDirectXCachedTexture : public ICachedTexture
{
public:
	CDirectXCachedTexture(CDirectXRenderer *renderer)
		: m_renderer(renderer)
	{
	}

	~CDirectXCachedTexture()
	{
		m_resourceView->Release();
		m_texture->Release();
	}

	virtual void Bind() const override
	{
		m_renderer->SetTextureResource(m_resourceView);
	}

	virtual void UnBind() const override
	{
		m_renderer->SetTextureResource(NULL);
	}

	operator ID3D11Texture2D**()
	{
		return &m_texture;
	}

	operator ID3D11ShaderResourceView**()
	{
		return &m_resourceView;
	}
private:
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
	CDirectXVertexBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon)
		:m_dev(dev), m_devcon(devcon)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = sizeof(unsigned int) * 10000;             // size is the index
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;       // use as a index buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

		m_dev->CreateBuffer(&bd, NULL, &m_pIndexBuffer);       // create the buffer
	}

	virtual void Bind() const override
	{
		UINT stride[] = { sizeof(float), sizeof(float), sizeof(float) };
		UINT offset[] = { 0, 0, 0 };
		ID3D11Buffer* buffers[] = { m_pVertexBuffer, m_pTexCoordBuffer, m_pNormalBuffer };
		m_devcon->IASetVertexBuffers(0, 3, buffers, stride, offset);	
		m_devcon->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		m_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	virtual void DrawIndexes(unsigned int * indexPtr, size_t count) override
	{		
		D3D11_MAPPED_SUBRESOURCE ms;
		m_devcon->Map(m_pIndexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
		memcpy(ms.pData, indexPtr, sizeof(unsigned int) * count);                // copy the data
		m_devcon->Unmap(m_pIndexBuffer, NULL);                        // unmap the buffer

		m_devcon->DrawIndexed(count, 0, 0);
	}

	virtual void DrawAll(size_t count) override
	{
		m_devcon->Draw(0, count);
	}

	virtual void UnBind() const override
	{
		m_devcon->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	}

	ID3D11Buffer ** GetVertexBuffer()
	{
		return &m_pVertexBuffer;
	}

	ID3D11Buffer ** GetNormalBuffer()
	{
		return &m_pNormalBuffer;
	}

	ID3D11Buffer ** GetTexCoordBuffer()
	{
		return &m_pTexCoordBuffer;
	}
private:
	CComPtr<ID3D11Buffer> m_pVertexBuffer;
	CComPtr<ID3D11Buffer> m_pTexCoordBuffer;
	CComPtr<ID3D11Buffer> m_pNormalBuffer;
	CComPtr<ID3D11Buffer> m_pIndexBuffer;
	ID3D11DeviceContext *m_devcon;
	ID3D11Device *m_dev;
};

class CDirectXFrameBuffer : public IFrameBuffer
{
public:
	virtual void Bind() const override
	{
	}

	virtual void UnBind() const override
	{
	}

	virtual void AssignTexture(ICachedTexture & texture, CachedTextureType type) override
	{
	}
private:
};


float GetAspectRatio(HWND m_hWnd)
{
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	return static_cast<float>(rect.right - rect.left) / static_cast<float>(rect.bottom - rect.top);
}

CDirectXRenderer::CDirectXRenderer(ID3D11Device *dev, ID3D11DeviceContext *devcon, HWND hWnd)
	:m_dev(dev), m_devcon(devcon), m_hWnd(hWnd)
	, m_defaultShaderManager(std::make_unique<CShaderManagerDirectX>(dev, devcon, this)), m_textureManager(*this), m_activeTextureSlot(0)
{
	m_viewMatrices.push_back(XMMatrixIdentity());
	float aspect = GetAspectRatio(m_hWnd);
	m_projectionMatrices.push_back(XMMatrixPerspectiveFovLH(60.0f, aspect, 0.5f, 1000.0f));

	m_defaultShaderManager->BindProgram();

	CreateBuffer(&m_vertexBuffer, sizeof(CVector3f) * 10000);
	CreateBuffer(&m_normalsBuffer, sizeof(CVector3f) * 10000);
	CreateBuffer(&m_texCoordBuffer, sizeof(CVector2f) * 10000);
	CreateBuffer(&m_weightBuffer, sizeof(float) * 4 * 10000);
}

void CDirectXRenderer::CreateBuffer(ID3D11Buffer ** bufferPtr, unsigned int elementSize)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = elementSize;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	m_dev->CreateBuffer(&bd, NULL, bufferPtr);       // create the buffer
}

void CDirectXRenderer::CreateBuffer(ID3D11Buffer ** bufferPtr, void * data, size_t size)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = size;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	m_dev->CreateBuffer(&bd, NULL, bufferPtr);       // create the buffer

	D3D11_MAPPED_SUBRESOURCE ms;
	m_devcon->Map(*bufferPtr, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, data, size);                // copy the data
	m_devcon->Unmap(*bufferPtr, NULL);                        // unmap the buffer
}

std::map<RenderMode, D3D11_PRIMITIVE_TOPOLOGY> renderModeMap = {
	{ RenderMode::LINES, D3D11_PRIMITIVE_TOPOLOGY_LINELIST },
	{ RenderMode::LINE_LOOP, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP },
	{ RenderMode::RECTANGLES, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
	{ RenderMode::TRIANGLES, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
	{ RenderMode::TRIANGLE_STRIP, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
};

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
	UINT stride[] = { sizeof(CVector2f), sizeof(CVector2f), 0 };
	UINT offset[] = { 0, 0, 0 };
	ID3D11Buffer* buffers[] = { m_vertexBuffer, m_texCoordBuffer, nullptr };

	std::vector<float> floatVertices;
	floatVertices.reserve(vertices.size() * 2);
	for (auto& vec : vertices)
	{
		floatVertices.push_back(vec.x);
		floatVertices.push_back(vec.y);
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	m_devcon->Map(m_vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, floatVertices.data(), floatVertices.size() * sizeof(float));                // copy the data
	m_devcon->Unmap(m_vertexBuffer, NULL);                        // unmap the buffer

	D3D11_MAPPED_SUBRESOURCE ms2;
	m_devcon->Map(m_texCoordBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);   // map the buffer
	memcpy(ms2.pData, texCoords.data(), texCoords.size() * sizeof(CVector2f));                // copy the data
	m_devcon->Unmap(m_texCoordBuffer, NULL);                        // unmap the buffer

	m_shaderManager->SetInputLayout(DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32_FLOAT);
	m_devcon->IASetVertexBuffers(0, 3, buffers, stride, offset);
	m_devcon->IASetPrimitiveTopology(renderModeMap.at(mode));
	m_devcon->Draw(vertices.size(), 0);
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords)
{
	UINT stride[] = { sizeof(CVector2f), sizeof(CVector2f) };
	UINT offset[] = { 0, 0 };
	ID3D11Buffer* buffers[] = { m_vertexBuffer, m_texCoordBuffer, nullptr };

	D3D11_MAPPED_SUBRESOURCE ms;
	m_devcon->Map(m_vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, vertices.data(), vertices.size() * sizeof(CVector2f));                // copy the data
	m_devcon->Unmap(m_vertexBuffer, NULL);                        // unmap the buffer

	D3D11_MAPPED_SUBRESOURCE ms2;
	m_devcon->Map(m_texCoordBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);   // map the buffer
	memcpy(ms2.pData, texCoords.data(), texCoords.size() * sizeof(CVector2f));                // copy the data
	m_devcon->Unmap(m_texCoordBuffer, NULL);                        // unmap the buffer

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
		result.push_back(item.x);
		result.push_back(item.y);
		result.push_back(item.z);
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
		floatTexCoords.push_back(item.x);
		floatTexCoords.push_back(item.y);
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	m_devcon->Map(m_vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, floatVertices.data(), floatVertices.size() * sizeof(float));                // copy the data
	m_devcon->Unmap(m_vertexBuffer, NULL);                        // unmap the buffer

	D3D11_MAPPED_SUBRESOURCE ms2;
	m_devcon->Map(m_texCoordBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);   // map the buffer
	memcpy(ms2.pData, floatTexCoords.data(), floatTexCoords.size() * sizeof(float));                // copy the data
	m_devcon->Unmap(m_texCoordBuffer, NULL);                        // unmap the buffer

	D3D11_MAPPED_SUBRESOURCE ms3;
	m_devcon->Map(m_normalsBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms3);   // map the buffer
	memcpy(ms3.pData, floatNormals.data(), floatNormals.size() * sizeof(float));                // copy the data
	m_devcon->Unmap(m_normalsBuffer, NULL);                        // unmap the buffer

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

	D3D11_MAPPED_SUBRESOURCE ms;
	m_devcon->Map(m_vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, vertices.data(), vertices.size() * sizeof(CVector2f));                // copy the data
	m_devcon->Unmap(m_vertexBuffer, NULL);                        // unmap the buffer

	D3D11_MAPPED_SUBRESOURCE ms2;
	m_devcon->Map(m_texCoordBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);   // map the buffer
	memcpy(ms2.pData, texCoords.data(), texCoords.size() * sizeof(CVector2f));                // copy the data
	m_devcon->Unmap(m_texCoordBuffer, NULL);                        // unmap the buffer

	D3D11_MAPPED_SUBRESOURCE ms3;
	m_devcon->Map(m_normalsBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms3);   // map the buffer
	memcpy(ms3.pData, normals.data(), normals.size() * sizeof(CVector2f));                // copy the data
	m_devcon->Unmap(m_normalsBuffer, NULL);                        // unmap the buffer

	m_shaderManager->SetInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT);
	m_devcon->IASetVertexBuffers(0, 3, buffers, stride, offset);
	m_devcon->IASetPrimitiveTopology(renderModeMap.at(mode));
	m_devcon->Draw(vertices.size(), 0);
}

void CDirectXRenderer::SetColor(const int * color)
{
	
}

void CDirectXRenderer::SetColor(const float * color)
{
	
}

void CDirectXRenderer::SetColor(const int r, const int g, const int b)
{
	
}

void CDirectXRenderer::SetColor(const float r, const float g, const float b)
{
	
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
	matrix *= XMMatrixTranslation(dx, dy, dz);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::Translate(const double dx, const double dy, const double dz)
{
	auto matrix = m_viewMatrices.back();
	matrix *= XMMatrixTranslation(dx, dy, dz);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::Translate(const float dx, const float dy, const float dz)
{
	auto matrix = m_viewMatrices.back();
	matrix *= XMMatrixTranslation(dx, dy, dz);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::Rotate(const double angle, const double x, const double y, const double z)
{
	XMVECTOR axis = { x, y, z, 1.0f };
	auto matrix = m_viewMatrices.back();
	matrix *= XMMatrixRotationAxis(axis, angle);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::Scale(const double scale)
{
	auto matrix = m_viewMatrices.back();
	matrix *= XMMatrixScaling(scale, scale, scale);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(matrix);
	UpdateMatrices();
}

void CDirectXRenderer::GetViewMatrix(float * matrix) const
{
	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view, m_viewMatrices.back());
	memcpy(matrix, *view.m, sizeof(view));
}

void CDirectXRenderer::ResetViewMatrix()
{
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(XMMatrixIdentity());
}

void CDirectXRenderer::LookAt(CVector3d const& position, CVector3d const& direction, CVector3d const& up)
{
	XMVECTOR pos = XMVectorSet(position.x, position.y, position.z, 1.0f);
	XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 1.0f);
	XMVECTOR upVec = XMVectorSet(up.x, up.y, up.z, 1.0f);
	m_viewMatrices.pop_back();
	m_viewMatrices.push_back(XMMatrixLookAtLH(pos, dir, upVec));
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
	ID3D11RenderTargetView* oldRenderTargetView;
	ID3D11DepthStencilView* oldDepthStencilView;
	m_devcon->OMGetRenderTargets(1, &oldRenderTargetView, &oldDepthStencilView);

	auto tex = std::make_unique<CDirectXCachedTexture>(this);
	CreateTexture(width, height, TEXTURE_HAS_ALPHA, NULL, *tex, *tex, true);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	CComPtr<ID3D11RenderTargetView> renderTargetView;
	ID3D11Texture2D ** texture = *tex;
	m_dev->CreateRenderTargetView(*texture, &renderTargetViewDesc, &renderTargetView);

	ID3D11RenderTargetView* view = renderTargetView;
	m_devcon->OMSetRenderTargets(1, &view, nullptr);

	func();

	m_devcon->OMSetRenderTargets(1, &oldRenderTargetView, oldDepthStencilView);
	if (oldRenderTargetView) oldRenderTargetView->Release();
	if (oldDepthStencilView) oldDepthStencilView->Release();

	return std::move(tex);
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type /*= CachedTextureType::RGBA*/)
{
	auto tex = std::make_unique<CDirectXCachedTexture>(this);
	CreateTexture(width, height, 0, data, *tex, *tex);
	return std::move(tex);
}

void CDirectXRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
	
}

std::unique_ptr<IDrawingList> CDirectXRenderer::CreateDrawingList(std::function<void() > const& func)
{
	return std::make_unique<CDirectXDrawingList>(func);
}

std::unique_ptr<IVertexBuffer> CDirectXRenderer::CreateVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/)
{
	auto buffer = std::make_unique<CDirectXVertexBuffer>(m_dev, m_devcon);

	CreateBuffer(buffer->GetVertexBuffer(), (void*)vertex, sizeof(vertex));
	CreateBuffer(buffer->GetNormalBuffer(), (void*)normals, sizeof(normals));
	CreateBuffer(buffer->GetTexCoordBuffer(), (void*)texcoords, sizeof(texcoords));

	return std::move(buffer);
}

std::unique_ptr<IShaderManager> CDirectXRenderer::CreateShaderManager() const
{
	return std::make_unique<CShaderManagerDirectX>(m_dev, m_devcon, const_cast<CDirectXRenderer *>(this));
}

void CDirectXRenderer::WindowCoordsToWorldVector(int x, int y, CVector3d & start, CVector3d & end) const
{
	
}

void CDirectXRenderer::WorldCoordsToWindowCoords(CVector3d const& worldCoords, int& x, int& y) const
{
	
}

std::unique_ptr<IFrameBuffer> CDirectXRenderer::CreateFramebuffer() const
{
	return std::make_unique<CDirectXFrameBuffer>();
}

void CDirectXRenderer::EnableLight(size_t index, bool enable)
{
	
}

void CDirectXRenderer::SetLightColor(size_t index, LightningType type, float * values)
{
	
}

void CDirectXRenderer::SetLightPosition(size_t index, float* pos)
{
	
}

float CDirectXRenderer::GetMaximumAnisotropyLevel() const
{
	return D3D11_REQ_MAXANISOTROPY;
}

void CDirectXRenderer::EnableVertexLightning(bool enable)
{
	
}

void CDirectXRenderer::GetProjectionMatrix(float * matrix) const
{
	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection, m_projectionMatrices.back());
	memcpy(matrix, *projection.m, sizeof(projection));
}

void CDirectXRenderer::EnableDepthTest(bool enable)
{
	
}

void CDirectXRenderer::EnableBlending(bool enable)
{
	
}

void CDirectXRenderer::SetUpViewport(CVector3d const& position, CVector3d const& target, unsigned int viewportWidth, unsigned int viewportHeight, double viewingAngle, double nearPane /*= 1.0*/, double farPane /*= 1000.0*/)
{
	m_projectionMatrices.push_back(XMMatrixPerspectiveFovLH(viewingAngle, static_cast<float>(viewportWidth) / viewportHeight, nearPane, farPane));
}

void CDirectXRenderer::RestoreViewport()
{
	m_projectionMatrices.pop_back();
}

void CDirectXRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	
}

void CDirectXRenderer::ClearBuffers(bool color /*= true*/, bool depth /*= true*/)
{
	ID3D11RenderTargetView * backbuffer;
	ID3D11DepthStencilView * depthBuffer;
	m_devcon->OMGetRenderTargets(1, &backbuffer, &depthBuffer);
	FLOAT backgroundColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	if(backbuffer && color) m_devcon->ClearRenderTargetView(backbuffer, backgroundColor);
	if (depthBuffer && depth) m_devcon->ClearDepthStencilView(depthBuffer, 0, 0.0f, 0);
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
	ID3D11ShaderResourceView* views[1] = { NULL };
	//m_devcon->PSSetShaderResources(m_activeTextureSlot, 1, views);
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::CreateEmptyTexture()
{
	return std::make_unique<CDirectXCachedTexture>(this);
}

void CDirectXRenderer::SetTextureAnisotropy(float value /*= 1.0f*/)
{
	ID3D11SamplerState* state;
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Filter = D3D11_FILTER_ANISOTROPIC;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = value;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MinLOD = 0;
	desc.MaxLOD = 0;
	m_dev->CreateSamplerState(&desc, &state);

	m_devcon->PSSetSamplers(0, 1, &state);

	state->Release();
}

void CDirectXRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
	auto& dxtexture = dynamic_cast<CDirectXCachedTexture&>(texture);
	CreateTexture(width, height, flags, data, dxtexture, dxtexture);
}

void CDirectXRenderer::UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, unsigned int width, unsigned int height, size_t size, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
	auto& dxtexture = dynamic_cast<CDirectXCachedTexture&>(texture);
	CreateTexture(width, height, flags, data, dxtexture, dxtexture);
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

void CDirectXRenderer::SetUpViewport2D()
{
	RECT rect;
	GetWindowRect(m_hWnd, &rect);
	m_projectionMatrices.push_back(XMMatrixOrthographicLH(rect.right - rect.left, rect.bottom - rect.top, -1.0f, 1.0f));
}

void CDirectXRenderer::SetTextureResource(ID3D11ShaderResourceView * view)
{
	ID3D11ShaderResourceView* views[1] = { view };
	//m_devcon->PSSetShaderResources(m_activeTextureSlot, 1, views);
}

void CDirectXRenderer::CreateTexture(unsigned int width, unsigned int height, int flags, const void * data, ID3D11Texture2D ** texture, ID3D11ShaderResourceView ** resourceView, bool renderTarget)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = flags & TEXTURE_BGRA ? ((flags & TEXTURE_HAS_ALPHA) ? DXGI_FORMAT_B8G8R8A8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM) : ((flags & TEXTURE_HAS_ALPHA) ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if (renderTarget) desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = renderTarget ? 0 : D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	int bpp = flags & TEXTURE_HAS_ALPHA ? 4 : 3;

	D3D11_SUBRESOURCE_DATA texData;
	texData.SysMemPitch = bpp * width;
	texData.pSysMem = data;

	m_dev->CreateTexture2D(&desc, data ? &texData : nullptr, texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC rDesc;
	ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	rDesc.Format = desc.Format;
	rDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	rDesc.Texture2D.MostDetailedMip = 0;
	rDesc.Texture2D.MipLevels = 1;

	m_dev->CreateShaderResourceView(*texture, &rDesc, resourceView);
}