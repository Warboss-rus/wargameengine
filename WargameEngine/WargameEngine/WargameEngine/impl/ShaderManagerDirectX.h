#pragma once
#include "../view/IShaderManager.h"
#include <d3d11.h>
#include <atlcomcli.h>
#include <d3d11shader.h>
#include <vector>
#include <map>
#include <functional>

class CDirectXRenderer;
struct sLightSource;

class CShaderManagerDirectX : public wargameEngine::view::IShaderManager
{
public:
	using Path = wargameEngine::Path;
	using IShaderProgram = wargameEngine::view::IShaderProgram;
	using IVertexAttribCache = wargameEngine::view::IVertexAttribCache;

	CShaderManagerDirectX(CDirectXRenderer * render);

	virtual std::unique_ptr<IShaderProgram> NewProgram(const Path& vertex = Path(), const Path& fragment = Path(), const Path& geometry = Path()) override;
	virtual std::unique_ptr<IShaderProgram> NewProgramSource(const std::string& vertex = "", const std::string& fragment = "", const std::string& geometry = "") override;
	virtual void PushProgram(IShaderProgram const& program) const override;
	virtual void PopProgram() const override;

	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance = false) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const override;

	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(size_t size, const void* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, int elementSize, size_t count, TYPE type, bool perInstance = false, size_t offset = 0) const override;

	void SetDevice(ID3D11Device* dev);
	void DoOnProgramChange(std::function<void()> const& handler);
	void SetInputLayout(DXGI_FORMAT vertexFormat, DXGI_FORMAT texCoordFormat, DXGI_FORMAT normalFormat) const;
	void SetColor(const float * color);
	void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess);
	void SetLight(size_t index, sLightSource & lightSource);
private:
	struct sConstantBuffer;
	class CDirectXShaderProgramImpl;
	class CDirectXShaderProgram;
	void SetUniformValueImpl(std::string const& uniform, int elementSize, size_t count, const void* value) const;
	void SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t count, DXGI_FORMAT format, bool perInstance, const void* values) const;
	void DisableAttributeImpl(std::string const& attribute) const;
	void CreateConstantBuffer(unsigned int size, ID3D11Buffer ** m_constantBuffer) const;
	void CopyConstantBufferData(sConstantBuffer const& buffer, unsigned int begin, const void * data, unsigned int size) const;
	void CreateBuffer(ID3D11Buffer ** bufferPtr, unsigned int size) const;
	void CopyBufferData(ID3D11Buffer * buffer, const void * data, unsigned int size) const;
	void MakeSureBufferCanFitData(CComPtr<ID3D11Buffer> & buffer, size_t totalSize, std::string const& attribute) const;
	void ResetBuffers() const;
	void ReflectConstantBuffers(ID3D10Blob * blob, CDirectXShaderProgramImpl & program, UINT stage);
	sConstantBuffer* FindBuffer(std::string const& uniform) const;

	ID3D11Device* m_dev;
	CDirectXRenderer * m_render;
	std::function<void()> m_onProgramChange;
	struct sConstantBuffer
	{
		CComPtr<ID3D11Buffer> m_constantBuffer;
		mutable std::vector<unsigned char> m_constantBufferData;
		std::map<std::string, size_t> m_variableOffsets;
		UINT m_startSlots[3] = {UINT_MAX, UINT_MAX, UINT_MAX};
	};
	class CDirectXShaderProgramImpl
	{
	public:
		CComPtr<ID3D11VertexShader> pVS;    // the vertex shader
		CComPtr<ID3D11PixelShader> pPS;     // the pixel shader
		CComPtr<ID3D11GeometryShader> pGS;     // the geometry shader
		CComPtr<ID3D10Blob> m_VS;

		std::map<std::string, sConstantBuffer> m_constantBuffers;

		mutable std::map<std::string, CComPtr<ID3D11Buffer>> m_vertexAttributeBuffers;
		mutable std::map<std::string, size_t> m_vertexAttributeBufferSizes;
		mutable std::map<std::string, D3D11_INPUT_ELEMENT_DESC> m_vertexAttributeDescriptions;
	};
	typedef std::tuple<std::wstring, std::wstring, std::wstring> ProgramCacheKey;
	std::map<ProgramCacheKey, CDirectXShaderProgramImpl> m_programsCache;
	typedef std::tuple<std::string, std::string, std::string> SourceProgramCacheKey;
	std::map<SourceProgramCacheKey, CDirectXShaderProgramImpl> m_sourceProgramCache;
	mutable std::vector<CDirectXShaderProgramImpl*> m_programs;
	mutable CDirectXShaderProgramImpl* m_activeProgram;

	typedef std::vector<std::pair<LPCSTR, DXGI_FORMAT>> InputLayoutDesc;
	mutable std::map<InputLayoutDesc, CComPtr<ID3D11InputLayout>> m_inputLayouts;
	mutable std::tuple<DXGI_FORMAT, DXGI_FORMAT, DXGI_FORMAT> m_lastInputLayout;
};