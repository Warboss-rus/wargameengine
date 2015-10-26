#include "ShaderManagerDirectX.h"
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "D3dcompiler.lib")

static const std::string defaultVertexShader = "\
float4 VShader( float4 Pos : POSITION ) : SV_POSITION\
{\
	return Pos;\
}";
static const std::string defaultPixelShader = "\
float4 PShader( float4 Pos : SV_POSITION ) : SV_Target\
{\
	return float4(1.0f, 1.0f, 1.0f, 0.0f);    // White\
}";
static const std::string defaultGeometryShader = "";

namespace
{
std::wstring Str2Wstr(std::string const& str)
{
	return std::wstring(str.begin(), str.end());
}
}

CShaderManagerDirectX::CShaderManagerDirectX(ID3D11Device *dev, ID3D11DeviceContext *devcon, IShaderManager * parent)
	:m_dev(dev), m_devcon(devcon), m_parent(parent)
{
}

void CompileShader(std::string const& path, char * entryPoint, std::string const& defaultContent, ID3D10Blob * blob)
{
	if (path.empty())
		D3DCompile(defaultContent.c_str(), defaultContent.size() + 1, "", NULL, NULL, entryPoint, "vs_4_0", 0, 0, &blob, NULL);
	else
		D3DCompileFromFile(Str2Wstr(path).c_str(), NULL, NULL, entryPoint, "vs_4_0", 0, 0, &blob, NULL);
}

void CShaderManagerDirectX::NewProgram(std::string const& vertex /*= ""*/, std::string const& fragment /*= ""*/, std::string const& geometry /*= ""*/)
{
	CComPtr<ID3D10Blob> VS, PS, GS;
	CompileShader(vertex, "VShader", defaultVertexShader, VS);
	CompileShader(fragment, "PShader", defaultPixelShader, PS);
	CompileShader(geometry, "GShader", defaultGeometryShader, GS);

	if (VS) m_dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	if (PS) m_dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);
	if (GS) m_dev->CreateGeometryShader(GS->GetBufferPointer(), GS->GetBufferSize(), NULL, &pGS);
}

void CShaderManagerDirectX::BindProgram() const
{
	m_devcon->VSSetShader(pVS, 0, 0);
	m_devcon->PSSetShader(pPS, 0, 0);
}

void CShaderManagerDirectX::UnBindProgram() const
{
	if (m_parent)
	{
		m_parent->BindProgram();
	}
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int count, const float* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int count, const int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int count, const unsigned int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue2(std::string const& uniform, int count, const float* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue2(std::string const& uniform, int count, const int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue2(std::string const& uniform, int count, const unsigned int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue3(std::string const& uniform, int count, const float* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue3(std::string const& uniform, int count, const int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue3(std::string const& uniform, int count, const unsigned int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue4(std::string const& uniform, int count, const float* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue4(std::string const& uniform, int count, const int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformValue4(std::string const& uniform, int count, const unsigned int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CShaderManagerDirectX::SetUniformMatrix4(std::string const& uniform, int count, float* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
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
