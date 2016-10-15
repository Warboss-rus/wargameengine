#include "ShaderManagerDirectX.h"
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "..\LogWriter.h"
#include "DirectXRenderer.h"
#include <DirectXMath.h>
#include "..\AsyncFileProvider.h"
#include "..\Utils.h"
#include <algorithm>
#include <iterator>

static const std::string defaultShader = "\
#define NUMBEROFLIGHTS 1\n\
struct sLightSource\
{\
	float3 pos;\
	int enabled;\
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
	sLightSource Lights[NUMBEROFLIGHTS];\
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
PixelInputType VShader( float3 Pos : POSITION, float2 texCoords : TEXCOORD, float3 normal : NORMAL)\
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

class CShaderManagerDirectX::CDirectXShaderProgram : public IShaderProgram
{
public:
	CDirectXShaderProgram(CDirectXShaderProgramImpl* implPtr)
		: impl(implPtr)
	{}
	CDirectXShaderProgramImpl* impl;
};

CShaderManagerDirectX::CShaderManagerDirectX(CDirectXRenderer * render)
	:m_render(render)
{
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

std::string ToLower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
	return str;
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

std::unique_ptr<IShaderProgram> CShaderManagerDirectX::NewProgram(std::wstring const& vertex /*= ""*/, std::wstring const& fragment /*= ""*/, std::wstring const& geometry /*= ""*/)
{
	ProgramCacheKey key = std::make_tuple(vertex, fragment, geometry);
	auto it = m_programsCache.find(key);
	if (it == m_programsCache.end())
	{
		it = m_programsCache.emplace(std::make_pair(key, CDirectXShaderProgramImpl())).first;
		auto& program = it->second;
		CComPtr<ID3D10Blob> PS, GS;
		CompileShader(vertex, "VShader", "vs_4_0", defaultShader, &program.m_VS);
		CompileShader(fragment, "PShader", "ps_4_0", defaultShader, &PS);
		CompileShader(geometry, "GShader", "gs_4_0", "", &GS);

		if (program.m_VS)
		{
			m_dev->CreateVertexShader(program.m_VS->GetBufferPointer(), program.m_VS->GetBufferSize(), NULL, &program.pVS);
		}
		if (PS)
		{
			m_dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &program.pPS);
		}
		if (GS)
		{
			m_dev->CreateGeometryShader(GS->GetBufferPointer(), GS->GetBufferSize(), NULL, &program.pGS);
		}

		if (program.m_VS)
		{
			CComPtr<ID3D11ShaderReflection> reflection;
			D3DReflect(program.m_VS->GetBufferPointer(), program.m_VS->GetBufferSize(),
				IID_ID3D11ShaderReflection, (void**)&reflection);

			auto constantBuffer = reflection->GetConstantBufferByName("Constant");
			D3D11_SHADER_BUFFER_DESC desc;
			constantBuffer->GetDesc(&desc);
			program.m_constantBufferData.resize(desc.Size, 0);
			program.m_constantBuffer = nullptr;
			CreateConstantBuffer(program.m_constantBufferData.size(), &program.m_constantBuffer);

			for (UINT i = 0; i < desc.Variables; ++i)
			{
				auto variable = constantBuffer->GetVariableByIndex(i);
				D3D11_SHADER_VARIABLE_DESC variableDesc;
				variable->GetDesc(&variableDesc);
				program.m_variableOffsets.emplace(std::make_pair(std::string(variableDesc.Name), static_cast<size_t>(variableDesc.StartOffset)));
			}
			D3D11_SHADER_DESC shaderDesc;
			reflection->GetDesc(&shaderDesc);
			D3D11_SIGNATURE_PARAMETER_DESC inputDesc;
			for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
			{
				reflection->GetInputParameterDesc(i, &inputDesc);
				D3D11_INPUT_ELEMENT_DESC elementDesc = { inputDesc.SemanticName, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, inputDesc.Register, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
				program.m_vertexAttributeDescriptions.emplace(std::make_pair(inputDesc.SemanticName, elementDesc));
				auto it2 = program.m_vertexAttributeDescriptions.find(inputDesc.SemanticName);
				it2->second.SemanticName = it2->first.c_str();
			}
		}
	}
	return std::make_unique<CDirectXShaderProgram>(&it->second);
}

void CShaderManagerDirectX::PushProgram(IShaderProgram const& program) const
{
	m_programs.push_back(reinterpret_cast<CDirectXShaderProgram const&>(program).impl);
	auto context = m_render->GetContext();
	context->VSSetShader(m_programs.back()->pVS, 0, 0);
	context->PSSetShader(m_programs.back()->pPS, 0, 0);
	context->GSSetShader(m_programs.back()->pGS, 0, 0);

	ResetBuffers();
	if(m_onProgramChange) m_onProgramChange();
}

void CShaderManagerDirectX::ResetBuffers() const
{
	std::vector<UINT> stride(m_programs.back()->m_vertexAttributeDescriptions.size(), 0);
	std::vector<UINT> offset(m_programs.back()->m_vertexAttributeDescriptions.size(), 0);
	std::vector<ID3D11Buffer*> buffers(m_programs.back()->m_vertexAttributeDescriptions.size(), NULL);
	m_render->GetContext()->IASetVertexBuffers(0, buffers.size(), buffers.data(), stride.data(), offset.data());
}

void CShaderManagerDirectX::PopProgram() const
{
	m_programs.pop_back();
	ResetBuffers();
	if (m_onProgramChange) m_onProgramChange();
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int count, const float* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(float) * count);
	}
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int count, const int* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(int) * count);
	}
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int count, const unsigned int* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(unsigned int) * count);
	}
}

void CShaderManagerDirectX::SetUniformValue2(std::string const& uniform, int count, const float* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(float) * count * 2);
	}
}

void CShaderManagerDirectX::SetUniformValue2(std::string const& uniform, int count, const int* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(int) * count * 2);
	}
}

void CShaderManagerDirectX::SetUniformValue2(std::string const& uniform, int count, const unsigned int* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(unsigned int) * count * 2);
	}
}

void CShaderManagerDirectX::SetUniformValue3(std::string const& uniform, int count, const float* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(float) * count * 3);
	}
}

void CShaderManagerDirectX::SetUniformValue3(std::string const& uniform, int count, const int* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(int) * count * 3);
	}
}

void CShaderManagerDirectX::SetUniformValue3(std::string const& uniform, int count, const unsigned int* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(unsigned int) * count * 3);
	}
}

void CShaderManagerDirectX::SetUniformValue4(std::string const& uniform, int count, const float* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(float) * count * 4);
	}
}

void CShaderManagerDirectX::SetUniformValue4(std::string const& uniform, int count, const int* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(int) * count * 4);
	}
}

void CShaderManagerDirectX::SetUniformValue4(std::string const& uniform, int count, const unsigned int* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(unsigned int) * count * 4);
	}
}

void CShaderManagerDirectX::SetUniformMatrix4(std::string const& uniform, size_t count, float* value) const
{
	auto it = m_programs.back()->m_variableOffsets.find(uniform);
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, value, sizeof(float) * count * 16);
	}
}

void CShaderManagerDirectX::SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, float* values) const
{
	auto descIt = m_programs.back()->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_programs.back()->m_vertexAttributeDescriptions.end())
	{
		CComPtr<ID3D11Buffer> buffer = m_programs.back()->m_vertexAttributeBuffers.find(attribute) != m_programs.back()->m_vertexAttributeBuffers.end() ? m_programs.back()->m_vertexAttributeBuffers[attribute] : nullptr;
		D3D11_INPUT_ELEMENT_DESC& desc = descIt->second;
		static const DXGI_FORMAT format[] = { DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };
		desc.Format = format[elementSize - 1];
		unsigned int stride = sizeof(float) * elementSize;
		unsigned int offset = 0;
		MakeSureBufferCanFitData(buffer, totalSize, attribute);
		CopyBufferData(buffer, values, totalSize * sizeof(float));
		m_render->GetContext()->IASetVertexBuffers(desc.InputSlot, 1, &buffer.p, &stride, &offset);
	}
}

void CShaderManagerDirectX::SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, int* values) const
{
	auto descIt = m_programs.back()->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_programs.back()->m_vertexAttributeDescriptions.end())
	{
		CComPtr<ID3D11Buffer> buffer = m_programs.back()->m_vertexAttributeBuffers.find(attribute) != m_programs.back()->m_vertexAttributeBuffers.end() ? m_programs.back()->m_vertexAttributeBuffers[attribute] : nullptr;
		D3D11_INPUT_ELEMENT_DESC& desc = descIt->second;
		static const DXGI_FORMAT format[] = { DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_SINT };
		desc.Format = format[elementSize - 1];
		unsigned int stride = sizeof(int) * elementSize;
		unsigned int offset = 0;
		MakeSureBufferCanFitData(buffer, totalSize, attribute);
		CopyBufferData(buffer, values, totalSize * sizeof(int));
		m_render->GetContext()->IASetVertexBuffers(desc.InputSlot, 1, &buffer.p, &stride, &offset);
	}
}

void CShaderManagerDirectX::SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, unsigned int* values) const
{
	auto descIt = m_programs.back()->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_programs.back()->m_vertexAttributeDescriptions.end())
	{
		CComPtr<ID3D11Buffer> buffer = m_programs.back()->m_vertexAttributeBuffers.find(attribute) != m_programs.back()->m_vertexAttributeBuffers.end() ? m_programs.back()->m_vertexAttributeBuffers[attribute] : nullptr;
		D3D11_INPUT_ELEMENT_DESC& desc = descIt->second;
		static const DXGI_FORMAT format[] = { DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT };
		desc.Format = format[elementSize - 1];
		unsigned int stride = sizeof(unsigned) * elementSize;
		unsigned int offset = 0;
		MakeSureBufferCanFitData(buffer, totalSize, attribute);
		CopyBufferData(buffer, values, totalSize * sizeof(unsigned));
		m_render->GetContext()->IASetVertexBuffers(desc.InputSlot, 1, &buffer.p, &stride, &offset);
	}
}

void CShaderManagerDirectX::SetPerInstanceVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, float* values) const
{
	auto descIt = m_programs.back()->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_programs.back()->m_vertexAttributeDescriptions.end())
	{
		CComPtr<ID3D11Buffer> buffer = m_programs.back()->m_vertexAttributeBuffers.find(attribute) != m_programs.back()->m_vertexAttributeBuffers.end() ? m_programs.back()->m_vertexAttributeBuffers[attribute] : nullptr;
		D3D11_INPUT_ELEMENT_DESC& desc = descIt->second;
		static const DXGI_FORMAT format[] = { DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };
		desc.Format = format[elementSize - 1];
		desc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
		desc.InstanceDataStepRate = 1;
		unsigned int stride = sizeof(float) * elementSize;
		unsigned int offset = 0;
		MakeSureBufferCanFitData(buffer, totalSize, attribute);
		CopyBufferData(buffer, values, totalSize * sizeof(float));
		m_render->GetContext()->IASetVertexBuffers(desc.InputSlot, 1, &buffer.p, &stride, &offset);
	}
}

void CShaderManagerDirectX::MakeSureBufferCanFitData(CComPtr<ID3D11Buffer> & buffer, size_t totalSize, std::string const& attribute) const
{
	if (!buffer || totalSize > m_programs.back()->m_vertexAttributeBufferSizes.at(attribute))
	{
		CreateBuffer(&buffer, totalSize * sizeof(float));
		m_programs.back()->m_vertexAttributeBufferSizes[attribute] = totalSize;
		m_programs.back()->m_vertexAttributeBuffers[attribute] = buffer;
	}
}

void CShaderManagerDirectX::DisableVertexAttribute(std::string const& attribute, int /*size*/, float* /*defaultValue*/) const
{
	auto descIt = m_programs.back()->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_programs.back()->m_vertexAttributeDescriptions.end())
	{
		unsigned int stride = sizeof(float) * 4;
		unsigned int offset = 0;
		ID3D11Buffer* buffers[] = { NULL };
		m_render->GetContext()->IASetVertexBuffers(descIt->second.InputSlot, 1, buffers, &stride, &offset);
	}
}

void CShaderManagerDirectX::DisableVertexAttribute(std::string const& attribute, int /*size*/, int* /*defaultValue*/) const
{
	auto descIt = m_programs.back()->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_programs.back()->m_vertexAttributeDescriptions.end())
	{
		unsigned int stride = sizeof(int);
		unsigned int offset = 0;
		ID3D11Buffer* buffers[] = { NULL };
		m_render->GetContext()->IASetVertexBuffers(descIt->second.InputSlot, 1, buffers, &stride, &offset);
	}
}

void CShaderManagerDirectX::DisableVertexAttribute(std::string const& attribute, int /*size*/, unsigned int* /*defaultValue*/) const
{
	auto descIt = m_programs.back()->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_programs.back()->m_vertexAttributeDescriptions.end())
	{
		unsigned int stride = sizeof(int);
		unsigned int offset = 0;
		ID3D11Buffer* buffers[] = { NULL };
		m_render->GetContext()->IASetVertexBuffers(descIt->second.InputSlot, 1, buffers, &stride, &offset);
	}
}

void CShaderManagerDirectX::SetDevice(ID3D11Device* dev)
{
	m_dev = dev;
	PushProgram(*NewProgram());
}

void CShaderManagerDirectX::DoOnProgramChange(std::function<void()> const& handler)
{
	m_onProgramChange = handler;
}

void CShaderManagerDirectX::SetInputLayout(DXGI_FORMAT vertexFormat, DXGI_FORMAT texCoordFormat, DXGI_FORMAT normalFormat) const
{
	// create the input layout object
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
	m_programs.back()->m_vertexAttributeDescriptions["POSITION"].Format = vertexFormat;
	m_programs.back()->m_vertexAttributeDescriptions["TEXCOORD"].Format = texCoordFormat;
	m_programs.back()->m_vertexAttributeDescriptions["NORMAL"].Format = normalFormat;
	for (auto& p : m_programs.back()->m_vertexAttributeDescriptions)
	{
		ied.push_back(p.second);
	}
	
	InputLayoutDesc key;
	std::transform(ied.begin(), ied.end(), std::back_inserter(key), [](D3D11_INPUT_ELEMENT_DESC const& desc) {return std::make_pair(desc.SemanticName, desc.Format);});
	auto it = m_inputLayouts.find(key);
	if(it == m_inputLayouts.end())
	{
		CComPtr<ID3D11InputLayout> pLayout;
		m_dev->CreateInputLayout(ied.data(), ied.size(), m_programs.back()->m_VS->GetBufferPointer(), m_programs.back()->m_VS->GetBufferSize(), &pLayout);
		it = m_inputLayouts.emplace(std::make_pair(key, std::move(pLayout))).first;
	}
	m_render->GetContext()->IASetInputLayout(it->second);
}

void CShaderManagerDirectX::SetMatrices(float * modelView, float * projection)
{
	DirectX::XMFLOAT4X4 fmatrix;
	auto it = m_programs.back()->m_variableOffsets.find("WorldViewProjection");
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		DirectX::XMMATRIX matrix = DirectX::XMMatrixMultiply(DirectX::XMMATRIX(modelView), DirectX::XMMATRIX(projection));
		matrix = DirectX::XMMatrixTranspose(matrix);
		DirectX::XMStoreFloat4x4(&fmatrix, matrix);
		CopyConstantBufferData(it->second, *fmatrix.m, sizeof(float) * 16);
	}

	it = m_programs.back()->m_variableOffsets.find("WorldView");
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		DirectX::XMMATRIX matrix = DirectX::XMMATRIX(modelView);
		matrix = DirectX::XMMatrixTranspose(matrix);
		DirectX::XMStoreFloat4x4(&fmatrix, matrix);
		CopyConstantBufferData(it->second, *fmatrix.m, sizeof(float) * 16);
	}
}

void CShaderManagerDirectX::SetColor(const float * color)
{
	auto it = m_programs.back()->m_variableOffsets.find("Color");
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second, color, sizeof(float) * 4);
	}
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
	auto it = m_programs.back()->m_variableOffsets.find("Material");
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		sMaterial material;
		memcpy(&material.AmbientColor, ambient, sizeof(float) * 4);
		memcpy(&material.DiffuseColor, diffuse, sizeof(float) * 4);
		memcpy(&material.SpecularColor, specular, sizeof(float) * 4);
		material.Shininess = shininess;
		CopyConstantBufferData(it->second, &material, sizeof(sMaterial));
	}
}

void CShaderManagerDirectX::SetLight(size_t index, sLightSource & lightSource)
{
	auto it = m_programs.back()->m_variableOffsets.find("Material");
	if (it != m_programs.back()->m_variableOffsets.end())
	{
		CopyConstantBufferData(it->second + index * sizeof(sLightSource), &lightSource, sizeof(sLightSource));
	}
}

void CShaderManagerDirectX::CopyConstantBufferData(unsigned int begin, const void * data, unsigned int size) const
{
	if (begin + size > m_programs.back()->m_constantBufferData.size()) return;
	memcpy(m_programs.back()->m_constantBufferData.data() + begin, data, size);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_render->GetContext()->Map(m_programs.back()->m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, m_programs.back()->m_constantBufferData.data(), m_programs.back()->m_constantBufferData.size());
	m_render->GetContext()->Unmap(m_programs.back()->m_constantBuffer, 0);
	m_render->GetContext()->VSSetConstantBuffers(0, 1, &m_programs.back()->m_constantBuffer.p);
	m_render->GetContext()->PSSetConstantBuffers(0, 1, &m_programs.back()->m_constantBuffer.p);
}

void CShaderManagerDirectX::CopyBufferData(ID3D11Buffer * buffer, const void * data, unsigned int size) const
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_render->GetContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, size);
	m_render->GetContext()->Unmap(buffer, 0);
}