#include "ShaderManagerDirectX.h"
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "..\LogWriter.h"
#include "DirectXRenderer.h"
#include <DirectXMath.h>

#pragma comment (lib, "D3dcompiler.lib")
#pragma comment (lib, "dxguid.lib")

static const std::string defaultVertexShader = "\
cbuffer Constant : register( b0 )\
{\
	matrix WorldViewProjection : WORLDVIEWPROJECTION;\
}\
\
float4 VShader( float3 Pos : POSITION, float2 texCoords : TEXCOORD, float3 normal : NORMAL, int4 indexes: WEIGHT_INDEX, float4 weight : WEIGHT ) : SV_POSITION\
{\
	return mul(float4(Pos, 1.0f), WorldViewProjection);\
}";
static const std::string defaultPixelShader = "\
float4 PShader( float4 Pos : SV_POSITION ) : SV_TARGET\
{\
	return float4(1.0f, 1.0f, 1.0f, 0.0f);\
}";
static const std::string defaultGeometryShader = "";

namespace
{
std::wstring Str2Wstr(std::string const& str)
{
	return std::wstring(str.begin(), str.end());
}
}

CShaderManagerDirectX::CShaderManagerDirectX(ID3D11Device *dev, ID3D11DeviceContext *devcon, CDirectXRenderer * render)
	:m_dev(dev), m_devcon(devcon), m_render(render)
{
	NewProgram();
}

void CompileShader(std::string const& path, char * entryPoint, char * target, std::string const& defaultContent, ID3D10Blob ** blob)
{
	CComPtr<ID3DBlob> errorBlob;
	if (path.empty())
		D3DCompile(defaultContent.c_str(), defaultContent.size() + 1, "", NULL, NULL, entryPoint, target, 0, 0, blob, &errorBlob);
	else
		D3DCompileFromFile(Str2Wstr(path).c_str(), NULL, NULL, entryPoint, target, 0, 0, blob, &errorBlob);
	if (errorBlob)
	{
		LogWriter::WriteLine("Error compiling shader " + path + ": " + (char*)errorBlob->GetBufferPointer());
	}
}

unsigned int GetShaderBufferSize(ID3D11ShaderReflectionConstantBuffer * buffer)
{
	D3D11_SHADER_BUFFER_DESC desc;
	buffer->GetDesc(&desc);
	return desc.Size;
}

void CreateConstantBuffer(ID3D11Device * dev, unsigned int size, ID3D11Buffer ** m_constantBuffer)
{
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = size;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	dev->CreateBuffer(&cbDesc, nullptr, m_constantBuffer);
}

void CShaderManagerDirectX::NewProgram(std::string const& vertex /*= ""*/, std::string const& fragment /*= ""*/, std::string const& geometry /*= ""*/)
{
	CComPtr<ID3D10Blob> PS, GS;
	CompileShader(vertex, "VShader", "vs_4_0", defaultVertexShader, &m_VS);
	CompileShader(fragment, "PShader", "ps_4_0", defaultPixelShader, &PS);
	CompileShader(geometry, "GShader", "gs_4_0", defaultGeometryShader, &GS);

	if (m_VS) m_dev->CreateVertexShader(m_VS->GetBufferPointer(), m_VS->GetBufferSize(), NULL, &pVS);
	if (PS) m_dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);
	if (GS) m_dev->CreateGeometryShader(GS->GetBufferPointer(), GS->GetBufferSize(), NULL, &pGS);

	D3DReflect(m_VS->GetBufferPointer(), m_VS->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&m_reflection);

	unsigned int size = GetShaderBufferSize(m_reflection->GetConstantBufferByName("Constant"));
	CreateConstantBuffer(m_dev, size, &m_constantBuffer);
	ID3D11Buffer * buffer = m_constantBuffer;
	m_devcon->VSSetConstantBuffers(0, 1, &buffer);
	m_devcon->PSSetConstantBuffers(0, 1, &buffer);
	m_devcon->GSSetConstantBuffers(0, 1, &buffer);
}

void CShaderManagerDirectX::BindProgram() const
{
	m_devcon->VSSetShader(pVS, 0, 0);
	m_devcon->PSSetShader(pPS, 0, 0);
	m_devcon->GSSetShader(pGS, 0, 0);

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

void CShaderManagerDirectX::SetUniformMatrix4(std::string const& uniform, int count, float* value) const
{
	unsigned int begin = GetVariableOffset("Constant", uniform);
	CopyConstantBufferData(begin, value, sizeof(float) * count * 16);
}

void CShaderManagerDirectX::SetVertexAttribute(eVertexAttribute attributeIndex, int size, float* values) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetVertexAttribute(eVertexAttribute attributeIndex, int size, int* values) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetVertexAttribute(eVertexAttribute attributeIndex, int size, unsigned int* values) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::DisableVertexAttribute(eVertexAttribute attributeIndex, int size, float* defaultValue) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::DisableVertexAttribute(eVertexAttribute attributeIndex, int size, int* defaultValue) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::DisableVertexAttribute(eVertexAttribute attributeIndex, int size, unsigned int* defaultValue) const
{
	throw std::logic_error("The method or operation is not implemented.");
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
	m_devcon->IASetInputLayout(pLayout);
	m_devcon->VSSetShader(pVS, 0, 0);
}

void CShaderManagerDirectX::SetMatrices(float * modelView, float * projection)
{
	DirectX::XMMATRIX modelViewMatrix(modelView);
	modelViewMatrix *= DirectX::XMMATRIX(projection);
	unsigned int begin = GetVariableOffset("Constant", "WorldViewProjection");

	CopyConstantBufferData(begin, &modelViewMatrix, sizeof(float) * 16);
}

void CShaderManagerDirectX::CopyConstantBufferData(unsigned int begin, const void * data, unsigned int size) const
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_devcon->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy((char*)mappedResource.pData + begin, data, size);
	m_devcon->Unmap(m_constantBuffer, 0);
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
