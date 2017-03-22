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
#include <array>

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
cbuffer Constant \
{\
	float4 Color;\
	sMaterial Material;\
	sLightSource Lights[NUMBEROFLIGHTS];\
};\
cbuffer VertexConstant\
{\
	matrix mvp_matrix : WORLDVIEWPROJECTION;\
};\
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
	result.position = mul(float4(Pos, 1.0f), mvp_matrix);\
	result.tex = texCoords;\
	return result;\
}\
float4 PShader( PixelInputType input) : SV_TARGET\
{\
	float4 tex = shaderTexture.Sample(SampleType, input.tex);\
	return float4(tex.xyz + Color.xyz, tex.a + Color.a);\
}";

class CShaderManagerDirectX::CDirectXShaderProgram : public IShaderProgram
{
public:
	CDirectXShaderProgram(CDirectXShaderProgramImpl* implPtr)
		: impl(implPtr)
	{}
	CDirectXShaderProgramImpl* impl;
};

class CVertexAttribCacheDirectX : public IVertexAttribCache
{
public:
	ID3D11Buffer* GetBuffer() const
	{
		return m_pBuffer;
	}
	ID3D11Buffer** GetBufferPtr()
	{
		return &m_pBuffer;
	}
private:
	CComPtr<ID3D11Buffer> m_pBuffer;
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
		if(!defaultContent.empty())
			D3DCompileFromFile(path.c_str(), NULL, NULL, entryPoint, target, 0, 0, blob, &errorBlob);
	if (errorBlob)
	{
		LogWriter::WriteLine(L"Error compiling shader " + path + L": " + Utf8ToWstring((char*)errorBlob->GetBufferPointer()));
	}
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

void CShaderManagerDirectX::ReflectConstantBuffers(ID3D10Blob * blob, CDirectXShaderProgramImpl & program, UINT stage)
{
	CComPtr<ID3D11ShaderReflection> reflection;
	D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**)&reflection);
	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);
	for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* constantBuffer = reflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC desc;
		constantBuffer->GetDesc(&desc);
		auto& programBufferDesc = program.m_constantBuffers[desc.Name];
		programBufferDesc.m_startSlots[stage] = i;
		programBufferDesc.m_constantBufferData.resize(desc.Size, 0);
		if (programBufferDesc.m_constantBufferData.size() < desc.Size || !programBufferDesc.m_constantBuffer)
		{
			CreateConstantBuffer(programBufferDesc.m_constantBufferData.size(), &programBufferDesc.m_constantBuffer);
		}

		for (UINT j = 0; j < desc.Variables; ++j)
		{
			auto variable = constantBuffer->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC variableDesc;
			variable->GetDesc(&variableDesc);
			programBufferDesc.m_variableOffsets.emplace(std::make_pair(std::string(variableDesc.Name), static_cast<size_t>(variableDesc.StartOffset)));
		}
	}
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

CShaderManagerDirectX::sConstantBuffer* CShaderManagerDirectX::FindBuffer(std::string const& uniform) const
{
	for (auto& buf : m_activeProgram->m_constantBuffers)
	{
		auto it = buf.second.m_variableOffsets.find(uniform);
		if (it != buf.second.m_variableOffsets.end())
		{
			return &buf.second;
		}
	}
	return nullptr;
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
		if(!geometry.empty()) CompileShader(geometry, "GShader", "gs_4_0", "", &GS);

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

		
		if (program.m_VS) ReflectConstantBuffers(program.m_VS, program, 0);
		if (PS) ReflectConstantBuffers(PS, program, 1);
		if (GS) ReflectConstantBuffers(GS, program, 1);
	}
	return std::make_unique<CDirectXShaderProgram>(&it->second);
}

void CShaderManagerDirectX::PushProgram(IShaderProgram const& program) const
{
	m_programs.push_back(reinterpret_cast<CDirectXShaderProgram const&>(program).impl);
	m_activeProgram = m_programs.back();
	auto context = m_render->GetContext();
	context->VSSetShader(m_activeProgram->pVS, 0, 0);
	context->PSSetShader(m_activeProgram->pPS, 0, 0);
	context->GSSetShader(m_activeProgram->pGS, 0, 0);

	ResetBuffers();
	if(m_onProgramChange) m_onProgramChange();
}

void CShaderManagerDirectX::ResetBuffers() const
{
	std::vector<UINT> stride(m_activeProgram->m_vertexAttributeDescriptions.size(), 0);
	std::vector<UINT> offset(m_activeProgram->m_vertexAttributeDescriptions.size(), 0);
	std::vector<ID3D11Buffer*> buffers(m_activeProgram->m_vertexAttributeDescriptions.size(), NULL);
	m_render->GetContext()->IASetVertexBuffers(0, buffers.size(), buffers.data(), stride.data(), offset.data());
	for (auto& buffer : m_activeProgram->m_constantBuffers)
	{
		if (buffer.second.m_startSlots[0] != UINT_MAX) m_render->GetContext()->VSSetConstantBuffers(buffer.second.m_startSlots[0], 1, &buffer.second.m_constantBuffer.p);
		if (buffer.second.m_startSlots[1] != UINT_MAX) m_render->GetContext()->PSSetConstantBuffers(buffer.second.m_startSlots[1], 1, &buffer.second.m_constantBuffer.p);
		if (buffer.second.m_startSlots[2] != UINT_MAX) m_render->GetContext()->GSSetConstantBuffers(buffer.second.m_startSlots[2], 1, &buffer.second.m_constantBuffer.p);
	}
}

void CShaderManagerDirectX::PopProgram() const
{
	m_programs.pop_back();
	m_activeProgram = m_programs.back();
	auto context = m_render->GetContext();
	context->VSSetShader(m_activeProgram->pVS, 0, 0);
	context->PSSetShader(m_activeProgram->pPS, 0, 0);
	context->GSSetShader(m_activeProgram->pGS, 0, 0);
	ResetBuffers();
	if (m_onProgramChange) m_onProgramChange();
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const
{
	SetUniformValueImpl(uniform, elementSize, count, value);
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const
{
	SetUniformValueImpl(uniform, elementSize, count, value);
}

void CShaderManagerDirectX::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const
{
	SetUniformValueImpl(uniform, elementSize, count, value);
}

void CShaderManagerDirectX::SetUniformValueImpl(std::string const& uniform, int elementSize, size_t count, const void* value) const
{
	auto buffer = FindBuffer(uniform);
	if (buffer)
	{
		auto it = buffer->m_variableOffsets.find(uniform);
		if (it != buffer->m_variableOffsets.end())
		{
			CopyConstantBufferData(*buffer, it->second, value, sizeof(unsigned int) * count * elementSize);
		}
	}
}

void CShaderManagerDirectX::SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t count, DXGI_FORMAT format, bool perInstance, const void* values) const
{
	auto descIt = m_activeProgram->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_activeProgram->m_vertexAttributeDescriptions.end())
	{
		CComPtr<ID3D11Buffer> buffer = m_activeProgram->m_vertexAttributeBuffers.find(attribute) != m_activeProgram->m_vertexAttributeBuffers.end() ? m_activeProgram->m_vertexAttributeBuffers[attribute] : nullptr;
		D3D11_INPUT_ELEMENT_DESC& desc = descIt->second;
		desc.Format = format;
		if (perInstance)
		{
			desc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			desc.InstanceDataStepRate = 1;
		}
		unsigned int stride = sizeof(float) * elementSize;
		unsigned int offset = 0;
		MakeSureBufferCanFitData(buffer, elementSize * count, attribute);
		CopyBufferData(buffer, values, elementSize * count * sizeof(float));
		m_render->GetContext()->IASetVertexBuffers(desc.InputSlot, 1, &buffer.p, &stride, &offset);
	}
}

std::unique_ptr<IVertexAttribCache> CShaderManagerDirectX::CreateVertexAttribCache(size_t size, const void* value) const
{
	auto result = std::make_unique<CVertexAttribCacheDirectX>();
	CreateBuffer(result->GetBufferPtr(), size);
	CopyBufferData(result->GetBuffer(), value, size);
	return std::move(result);
}

void CShaderManagerDirectX::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance) const
{
	static const DXGI_FORMAT format[] = { DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };
	SetVertexAttributeImpl(attribute, elementSize, count, format[elementSize - 1], perInstance, values);
}

void CShaderManagerDirectX::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance) const
{
	static const DXGI_FORMAT format[] = { DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_SINT };
	SetVertexAttributeImpl(attribute, elementSize, count, format[elementSize - 1], perInstance, values);
}

void CShaderManagerDirectX::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance) const
{
	static const DXGI_FORMAT format[] = { DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT };
	SetVertexAttributeImpl(attribute, elementSize, count, format[elementSize - 1], perInstance, values);
}

void CShaderManagerDirectX::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, int elementSize, size_t /*count*/, TYPE type, bool perInstance /*= false*/, size_t offset /*= 0*/) const
{
	auto& dxCache = reinterpret_cast<CVertexAttribCacheDirectX const&>(cache);
	auto descIt = m_activeProgram->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_activeProgram->m_vertexAttributeDescriptions.end())
	{
		static const std::map<TYPE, std::array<DXGI_FORMAT, 4>> typeMap = {
			{ TYPE::FLOAT32, { DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT } },
			{ TYPE::SINT32, { DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_SINT } },
			{ TYPE::UINT32, {DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT } },
		};
		D3D11_INPUT_ELEMENT_DESC& desc = descIt->second;
		desc.Format = typeMap.at(type).at(elementSize - 1);
		if (perInstance)
		{
			desc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			desc.InstanceDataStepRate = 1;
		}
		unsigned int stride = sizeof(float) * elementSize;
		UINT uintOffset = static_cast<UINT>(offset);
		auto buffer = dxCache.GetBuffer();
		m_render->GetContext()->IASetVertexBuffers(desc.InputSlot, 1, &buffer, &stride, &uintOffset);
	}
}

void CShaderManagerDirectX::MakeSureBufferCanFitData(CComPtr<ID3D11Buffer> & buffer, size_t totalSize, std::string const& attribute) const
{
	if (!buffer || totalSize > m_activeProgram->m_vertexAttributeBufferSizes.at(attribute))
	{
		CreateBuffer(&buffer, totalSize * sizeof(float));
		m_activeProgram->m_vertexAttributeBufferSizes[attribute] = totalSize;
		m_activeProgram->m_vertexAttributeBuffers[attribute] = buffer;
	}
}

void CShaderManagerDirectX::DisableAttributeImpl(std::string const& attribute) const
{
	auto descIt = m_activeProgram->m_vertexAttributeDescriptions.find(attribute);
	if (descIt != m_activeProgram->m_vertexAttributeDescriptions.end())
	{
		unsigned int stride = sizeof(float) * 4;
		unsigned int offset = 0;
		ID3D11Buffer* buffers[] = { NULL };
		m_render->GetContext()->IASetVertexBuffers(descIt->second.InputSlot, 1, buffers, &stride, &offset);
	}
}

void CShaderManagerDirectX::DisableVertexAttribute(std::string const& attribute, int /*size*/, const float* /*defaultValue*/) const
{
	DisableAttributeImpl(attribute);
}

void CShaderManagerDirectX::DisableVertexAttribute(std::string const& attribute, int /*size*/, const int* /*defaultValue*/) const
{
	DisableAttributeImpl(attribute);
}

void CShaderManagerDirectX::DisableVertexAttribute(std::string const& attribute, int /*size*/, const unsigned int* /*defaultValue*/) const
{
	DisableAttributeImpl(attribute);
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
	auto tuple = std::make_tuple(vertexFormat, texCoordFormat, normalFormat);
	if (tuple == m_lastInputLayout) return;
	m_lastInputLayout = tuple;
	// create the input layout object
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
	m_activeProgram->m_vertexAttributeDescriptions["POSITION"].Format = vertexFormat;
	m_activeProgram->m_vertexAttributeDescriptions["TEXCOORD"].Format = texCoordFormat;
	m_activeProgram->m_vertexAttributeDescriptions["NORMAL"].Format = normalFormat;
	for (auto& p : m_activeProgram->m_vertexAttributeDescriptions)
	{
		ied.push_back(p.second);
	}
	
	InputLayoutDesc key;
	std::transform(ied.begin(), ied.end(), std::back_inserter(key), [](D3D11_INPUT_ELEMENT_DESC const& desc) {return std::make_pair(desc.SemanticName, desc.Format);});
	auto it = m_inputLayouts.find(key);
	if(it == m_inputLayouts.end())
	{
		CComPtr<ID3D11InputLayout> pLayout;
		m_dev->CreateInputLayout(ied.data(), ied.size(), m_activeProgram->m_VS->GetBufferPointer(), m_activeProgram->m_VS->GetBufferSize(), &pLayout);
		it = m_inputLayouts.emplace(std::make_pair(key, std::move(pLayout))).first;
	}
	m_render->GetContext()->IASetInputLayout(it->second);
}

void CShaderManagerDirectX::SetColor(const float * color)
{
	static const std::string colorKey = "Color";
	auto buffer = FindBuffer(colorKey);
	auto it = buffer->m_variableOffsets.find(colorKey);
	if (it != buffer->m_variableOffsets.end())
	{
		CopyConstantBufferData(*buffer, it->second, color, sizeof(float) * 4);
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
	static const std::string materialKey = "Material";
	auto buffer = FindBuffer(materialKey);
	auto it = buffer->m_variableOffsets.find(materialKey);
	if (it != buffer->m_variableOffsets.end())
	{
		sMaterial material;
		memcpy(&material.AmbientColor, ambient, sizeof(float) * 4);
		memcpy(&material.DiffuseColor, diffuse, sizeof(float) * 4);
		memcpy(&material.SpecularColor, specular, sizeof(float) * 4);
		material.Shininess = shininess;
		CopyConstantBufferData(*buffer, it->second, &material, sizeof(sMaterial));
	}
}

void CShaderManagerDirectX::SetLight(size_t index, sLightSource & lightSource)
{
	static const std::string lightKey = "Lights";
	auto buffer = FindBuffer(lightKey);
	auto it = buffer->m_variableOffsets.find(lightKey);
	if (it != buffer->m_variableOffsets.end())
	{
		CopyConstantBufferData(*buffer, it->second + index * sizeof(sLightSource), &lightSource, sizeof(sLightSource));
	}
}

void CShaderManagerDirectX::CopyConstantBufferData(sConstantBuffer const& buffer, unsigned int begin, const void * data, unsigned int size) const
{
	if (begin + size > buffer.m_constantBufferData.size()) return;
	memcpy(buffer.m_constantBufferData.data() + begin, data, size);
	CopyBufferData(buffer.m_constantBuffer, buffer.m_constantBufferData.data(), buffer.m_constantBufferData.size());
}

void CShaderManagerDirectX::CopyBufferData(ID3D11Buffer * buffer, const void * data, unsigned int size) const
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_render->GetContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, size);
	m_render->GetContext()->Unmap(buffer, 0);
}