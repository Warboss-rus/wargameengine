#include "ShaderManagerDirectX.h"
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "..\LogWriter.h"
#include "DirectXRenderer.h"
#include <DirectXMath.h>
#include "..\AsyncFileProvider.h"
#include "..\Utils.h"

static const std::string defaultShader = "\
struct sLightSource\
{\
	bool enabled;\
	float4 pos;\
	float4 diffuse;\
	float4 ambient;\
	float4 specular;\
};\
struct sMaterial\
{\
	float4 AmbientColor;\
	float4 DiffuseColor;\
	float4 SpecularColor;\
	float Shininess;\
};\
cbuffer Constant : register( b0 )\
{\
	matrix WorldViewProjection : WORLDVIEWPROJECTION;\
	float4 Color;\
	sMaterial Material;\
	sLightSource Lights;\
}\
struct PixelInputType\
{\
	float4 position : SV_POSITION;\
	float2 tex : TEXCOORD0;\
};\
Texture2D shaderTexture : register( t0 );\
Texture2D shadowTexture : register( t1 );\
SamplerState SampleType;\
\
PixelInputType VShader( float3 Pos : POSITION, float2 texCoords : TEXCOORD, float3 normal : NORMAL, int4 indexes: WEIGHT_INDEX, float4 weight : WEIGHT )\
{\
	PixelInputType result;\
	result.position = mul(float4(Pos, 1.0f), WorldViewProjection);\
	result.tex = texCoords;\
	return result;\
}\
float4 PShader( PixelInputType input) : SV_TARGET\
{\
	float4 tex = shaderTexture.Sample(SampleType, input.tex);\
	return tex + Color;\
}";

CShaderManagerDirectX::CShaderManagerDirectX(ID3D11Device *dev, CDirectXRenderer * render)
	:m_dev(dev), m_render(render)
{
	NewProgram();
}

void CompileShader(std::wstring const& path, char * entryPoint, char * target, std::string const& defaultContent, ID3D10Blob ** blob)
{
	CComPtr<ID3DBlob> errorBlob;
	if (path.empty())
		D3DCompile(defaultContent.c_str(), defaultContent.size() + 1, "", NULL, NULL, entryPoint, target, 0, 0, blob, &errorBlob);
	else
		
		D3DCompileFromFile(path.c_str(), NULL, NULL, entryPoint, target, 0, 0, blob, &errorBlob);
	if (errorBlob)
	{
		LogWriter::WriteLine(L"Error compiling shader " + path + L": " + Utf8ToWstring((char*)errorBlob->GetBufferPointer()));
	}
}

unsigned int GetShaderBufferSize(ID3D11ShaderReflectionConstantBuffer * buffer)
{
	D3D11_SHADER_BUFFER_DESC desc;
	buffer->GetDesc(&desc);
	return desc.Size;
}

void CShaderManagerDirectX::CreateConstantBuffer(unsigned int size, ID3D11Buffer ** constantBuffer) const
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = size;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	m_dev->CreateBuffer(&cbDesc, nullptr, constantBuffer);
}

void CShaderManagerDirectX::CreateBuffer(ID3D11Buffer ** bufferPtr, unsigned int size) const
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = size;             // size
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	m_dev->CreateBuffer(&bd, NULL, bufferPtr);       // create the buffer
}

void CShaderManagerDirectX::NewProgram(std::wstring const& vertex /*= ""*/, std::wstring const& fragment /*= ""*/, std::wstring const& geometry /*= ""*/)
{
	if (!vertex.empty()) m_VS = nullptr;
	CComPtr<ID3D10Blob> PS, GS;
	CompileShader(vertex, "VShader", "vs_4_0", defaultShader, &m_VS);
	CompileShader(fragment, "PShader", "ps_4_0", defaultShader, &PS);
	CompileShader(geometry, "GShader", "gs_4_0", "", &GS);

	if (m_VS)
	{
		pVS = nullptr;
		m_dev->CreateVertexShader(m_VS->GetBufferPointer(), m_VS->GetBufferSize(), NULL, &pVS);
	}
	if (PS)
	{
		pPS = nullptr;
		m_dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);
	}
	if (GS)
	{
		pGS = nullptr;
		m_dev->CreateGeometryShader(GS->GetBufferPointer(), GS->GetBufferSize(), NULL, &pGS);
	}

	m_reflection = nullptr;
	if (m_VS)
	{
		D3DReflect(m_VS->GetBufferPointer(), m_VS->GetBufferSize(),
			IID_ID3D11ShaderReflection, (void**)&m_reflection);

		unsigned int size = GetShaderBufferSize(m_reflection->GetConstantBufferByName("Constant"));
		m_constantBufferData.resize(size);
		memset(m_constantBufferData.data(), 0, m_constantBufferData.size());
		m_constantBuffer = nullptr;
		CreateConstantBuffer(m_constantBufferData.size(), &m_constantBuffer);
	}
}

void CShaderManagerDirectX::BindProgram() const
{
	auto context = m_render->GetContext();
	context->VSSetShader(pVS, 0, 0);
	context->PSSetShader(pPS, 0, 0);
	context->GSSetShader(pGS, 0, 0);

	m_render->SetShaderManager((CShaderManagerDirectX*)this);
}

void CShaderManagerDirectX::UnBindProgram() const
{
	m_render->SetShaderManager(nullptr);
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int count, const float* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(float) * count);
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int count, const int* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(int) * count);
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int count, const unsigned int* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(unsigned int) * count);
}

void CShaderManagerDirectX::SetUniformValue2(std::string const& uniform, int count, const float* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(float) * count * 2);
}

void CShaderManagerDirectX::SetUniformValue2(std::string const& uniform, int count, const int* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(int) * count * 2);
}

void CShaderManagerDirectX::SetUniformValue2(std::string const& uniform, int count, const unsigned int* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(unsigned int) * count * 2);
}

void CShaderManagerDirectX::SetUniformValue3(std::string const& uniform, int count, const float* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(float) * count * 3);
}

void CShaderManagerDirectX::SetUniformValue3(std::string const& uniform, int count, const int* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(int) * count * 3);
}

void CShaderManagerDirectX::SetUniformValue3(std::string const& uniform, int count, const unsigned int* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(unsigned int) * count * 3);
}

void CShaderManagerDirectX::SetUniformValue4(std::string const& uniform, int count, const float* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(float) * count * 4);
}

void CShaderManagerDirectX::SetUniformValue4(std::string const& uniform, int count, const int* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(int) * count * 4);
}

void CShaderManagerDirectX::SetUniformValue4(std::string const& uniform, int count, const unsigned int* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(unsigned int) * count * 4);
}

void CShaderManagerDirectX::SetUniformMatrix4(std::string const& uniform, size_t count, float* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(float) * count * 16);
}

void CShaderManagerDirectX::SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t totalSize, float* values) const
{
	CComPtr<ID3D11Buffer> buffer = m_vertexAttributeBuffers.find(attributeIndex) != m_vertexAttributeBuffers.end() ? m_vertexAttributeBuffers[attributeIndex] : nullptr;
	unsigned int index = attributeIndex == eVertexAttribute::WEIGHT ? 4 : 3;
	unsigned int stride = sizeof(float) * elementSize;
	unsigned int offset = 0;
	MakeSureBufferCanFitData(buffer, totalSize, attributeIndex);
	CopyBufferData(buffer, values, totalSize * sizeof(float));
	m_render->GetContext()->IASetVertexBuffers(index, 1, &buffer.p, &stride, &offset);
}

void CShaderManagerDirectX::SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t totalSize, int* values) const
{
	CComPtr<ID3D11Buffer> buffer = m_vertexAttributeBuffers.find(attributeIndex) != m_vertexAttributeBuffers.end() ? m_vertexAttributeBuffers[attributeIndex] : nullptr;
	unsigned int index = attributeIndex == eVertexAttribute::WEIGHT ? 4 : 3;
	unsigned int stride = sizeof(float) * elementSize;
	unsigned int offset = 0;
	MakeSureBufferCanFitData(buffer, totalSize, attributeIndex);
	CopyBufferData(buffer, values, totalSize * sizeof(int));
	m_render->GetContext()->IASetVertexBuffers(index, 1, &buffer.p, &stride, &offset);
}

void CShaderManagerDirectX::SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t totalSize, unsigned int* values) const
{
	CComPtr<ID3D11Buffer> buffer = m_vertexAttributeBuffers.find(attributeIndex) != m_vertexAttributeBuffers.end() ? m_vertexAttributeBuffers[attributeIndex] : nullptr;
	unsigned int index = attributeIndex == eVertexAttribute::WEIGHT ? 4 : 3;
	unsigned int stride = sizeof(float) * elementSize;
	unsigned int offset = 0;
	MakeSureBufferCanFitData(buffer, totalSize, attributeIndex);
	CopyBufferData(buffer, values, totalSize * sizeof(unsigned int));
	m_render->GetContext()->IASetVertexBuffers(index, 1, &buffer.p, &stride, &offset);
}

void CShaderManagerDirectX::MakeSureBufferCanFitData(CComPtr<ID3D11Buffer> & buffer, size_t totalSize, eVertexAttribute attributeIndex) const
{
	if (!buffer || totalSize > m_vertexAttributeBufferSizes.at(attributeIndex))
	{
		CreateBuffer(&buffer, totalSize * sizeof(float));
		m_vertexAttributeBufferSizes[attributeIndex] = totalSize;
		m_vertexAttributeBuffers[attributeIndex] = buffer;
	}
}

void CShaderManagerDirectX::DisableVertexAttribute(eVertexAttribute attributeIndex, int /*size*/, float* /*defaultValue*/) const
{
	unsigned int index = attributeIndex == eVertexAttribute::WEIGHT ? 4 : 3;
	unsigned int stride = sizeof(float) * 4;
	unsigned int offset = 0;
	ID3D11Buffer* buffers[] = { NULL };
	m_render->GetContext()->IASetVertexBuffers(index, 1, buffers, &stride, &offset);
}

void CShaderManagerDirectX::DisableVertexAttribute(eVertexAttribute attributeIndex, int /*size*/, int* /*defaultValue*/) const
{
	unsigned int index = attributeIndex == eVertexAttribute::WEIGHT ? 4 : 3;
	unsigned int stride = sizeof(int);
	unsigned int offset = 0;
	ID3D11Buffer* buffers[] = { NULL };
	m_render->GetContext()->IASetVertexBuffers(index, 1, buffers, &stride, &offset);
}

void CShaderManagerDirectX::DisableVertexAttribute(eVertexAttribute attributeIndex, int /*size*/, unsigned int* /*defaultValue*/) const
{
	unsigned int index = attributeIndex == eVertexAttribute::WEIGHT ? 4 : 3;
	unsigned int stride = sizeof(int);
	unsigned int offset = 0;
	ID3D11Buffer* buffers[] = { NULL };
	m_render->GetContext()->IASetVertexBuffers(index, 1, buffers, &stride, &offset);
}

void CShaderManagerDirectX::SetInputLayout(DXGI_FORMAT vertexFormat, DXGI_FORMAT texCoordFormat, DXGI_FORMAT normalFormat)
{
	// create the input layout object
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
	ied.push_back({ "POSITION", 0, vertexFormat, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	ied.push_back({ "TEXCOORD", 0, texCoordFormat, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	ied.push_back({ "NORMAL", 0, normalFormat, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	ied.push_back({ "WEIGHT_INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	ied.push_back({ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	CComPtr<ID3D11InputLayout> pLayout;
	m_dev->CreateInputLayout(ied.data(), ied.size(), m_VS->GetBufferPointer(), m_VS->GetBufferSize(), &pLayout);
	m_render->GetContext()->IASetInputLayout(pLayout);
	m_render->GetContext()->VSSetShader(pVS, 0, 0);
}

void CShaderManagerDirectX::SetMatrices(float * modelView, float * projection)
{
	DirectX::XMMATRIX matrix = DirectX::XMMatrixMultiply(DirectX::XMMATRIX(modelView), DirectX::XMMATRIX(projection));
	matrix = DirectX::XMMatrixTranspose(matrix);
	DirectX::XMFLOAT4X4 fmatrix;
	DirectX::XMStoreFloat4x4(&fmatrix, matrix);
	unsigned int begin = GetVariableOffset("Constant", "WorldViewProjection");

	CopyConstantBufferData(begin, *fmatrix.m, sizeof(float) * 16);
}

void CShaderManagerDirectX::SetColor(const float * color)
{
	auto begin = GetVariableOffset("Constant", "Color");
	CopyConstantBufferData(begin, color, sizeof(float) * 4);
}

struct sMaterial
{
	float AmbientColor[4];
	float DiffuseColor[4];
	float SpecularColor[4];
	float Shininess;
};

void CShaderManagerDirectX::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
	sMaterial material;
	memcpy(&material.AmbientColor, ambient, sizeof(float) * 4);
	memcpy(&material.DiffuseColor, diffuse, sizeof(float) * 4);
	memcpy(&material.SpecularColor, specular, sizeof(float) * 4);
	material.Shininess = shininess;
	CopyConstantBufferData(GetVariableOffset("Constant", "Material"), &material, sizeof(sMaterial));
}

void CShaderManagerDirectX::SetLight(size_t index, sLightSource & lightSource)
{
	CopyConstantBufferData(GetVariableOffset("Constant", "Lights") + index * sizeof(sLightSource), &lightSource, sizeof(sLightSource));
}

void CShaderManagerDirectX::CopyConstantBufferData(unsigned int begin, const void * data, unsigned int size) const
{
	if (begin + size > m_constantBufferData.size()) return;
	memcpy(m_constantBufferData.data() + begin, data, size);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_render->GetContext()->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, m_constantBufferData.data(), m_constantBufferData.size());
	m_render->GetContext()->Unmap(m_constantBuffer, 0);
	m_render->GetContext()->VSSetConstantBuffers(0, 1, &m_constantBuffer.p);
	m_render->GetContext()->PSSetConstantBuffers(0, 1, &m_constantBuffer.p);
}

void CShaderManagerDirectX::CopyBufferData(ID3D11Buffer * buffer, const void * data, unsigned int size) const
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_render->GetContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, size);
	m_render->GetContext()->Unmap(buffer, 0);
}

unsigned int CShaderManagerDirectX::GetVariableOffset(std::string const& bufferName, std::string const& name, unsigned int * size) const
{
	auto buffer = m_reflection->GetConstantBufferByName(bufferName.c_str());
	auto variable = buffer->GetVariableByName(name.c_str());
	D3D11_SHADER_VARIABLE_DESC vdesc;
	variable->GetDesc(&vdesc);
	if (size) *size = vdesc.Size;
	return vdesc.StartOffset;
}
