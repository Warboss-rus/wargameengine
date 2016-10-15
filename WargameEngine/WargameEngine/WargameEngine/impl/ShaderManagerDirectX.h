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

class CShaderManagerDirectX : public IShaderManager
{
public:
	CShaderManagerDirectX(CDirectXRenderer * render);

	virtual std::unique_ptr<IShaderProgram> NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") override;
	virtual void PushProgram(IShaderProgram const& program) const override;
	virtual void PopProgram() const override;

	virtual void SetUniformValue(std::string const& uniform, int count, const float* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int count, const int* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int count, const unsigned int* value) const override;
	virtual void SetUniformValue2(std::string const& uniform, int count, const float* value) const override;
	virtual void SetUniformValue2(std::string const& uniform, int count, const int* value) const override;
	virtual void SetUniformValue2(std::string const& uniform, int count, const unsigned int* value) const override;
	virtual void SetUniformValue3(std::string const& uniform, int count, const float* value) const override;
	virtual void SetUniformValue3(std::string const& uniform, int count, const int* value) const override;
	virtual void SetUniformValue3(std::string const& uniform, int count, const unsigned int* value) const override;
	virtual void SetUniformValue4(std::string const& uniform, int count, const float* value) const override;
	virtual void SetUniformValue4(std::string const& uniform, int count, const int* value) const override;
	virtual void SetUniformValue4(std::string const& uniform, int count, const unsigned int* value) const override;
	virtual void SetUniformMatrix4(std::string const& uniform, size_t count, float* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, float* values) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, int* values) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, unsigned int* values) const override;
	virtual void SetPerInstanceVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, float* values) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, float* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, int* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, unsigned int* defaultValue) const override;

	void SetDevice(ID3D11Device* dev);
	void DoOnProgramChange(std::function<void()> const& handler);
	void SetInputLayout(DXGI_FORMAT vertexFormat, DXGI_FORMAT texCoordFormat, DXGI_FORMAT normalFormat) const;
	void SetMatrices(float * modelView, float * projection);
	void SetColor(const float * color);
	void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess);
	void SetLight(size_t index, sLightSource & lightSource);
private:
	void CreateConstantBuffer(unsigned int size, ID3D11Buffer ** m_constantBuffer) const;
	void CopyConstantBufferData(unsigned int begin, const void * data, unsigned int size) const;
	void CreateBuffer(ID3D11Buffer ** bufferPtr, unsigned int size) const;
	void CopyBufferData(ID3D11Buffer * buffer, const void * data, unsigned int size) const;
	void MakeSureBufferCanFitData(CComPtr<ID3D11Buffer> & buffer, size_t totalSize, std::string const& attribute) const;
	void ResetBuffers() const;

	ID3D11Device* m_dev;
	CDirectXRenderer * m_render;
	std::function<void()> m_onProgramChange;
	class CDirectXShaderProgram;
	class CDirectXShaderProgramImpl
	{
	public:
		CComPtr<ID3D11VertexShader> pVS;    // the vertex shader
		CComPtr<ID3D11PixelShader> pPS;     // the pixel shader
		CComPtr<ID3D11GeometryShader> pGS;     // the geometry shader
		CComPtr<ID3D10Blob> m_VS;

		CComPtr<ID3D11Buffer> m_constantBuffer;
		mutable std::vector<unsigned char> m_constantBufferData;
		std::map<std::string, size_t> m_variableOffsets;

		mutable std::map<std::string, CComPtr<ID3D11Buffer>> m_vertexAttributeBuffers;
		mutable std::map<std::string, size_t> m_vertexAttributeBufferSizes;
		mutable std::map<std::string, D3D11_INPUT_ELEMENT_DESC> m_vertexAttributeDescriptions;
	};
	typedef std::tuple<std::wstring, std::wstring, std::wstring> ProgramCacheKey;
	std::map<ProgramCacheKey, CDirectXShaderProgramImpl> m_programsCache;
	mutable std::vector<CDirectXShaderProgramImpl*> m_programs;

	typedef std::vector<std::pair<LPCSTR, DXGI_FORMAT>> InputLayoutDesc;
	mutable std::map<InputLayoutDesc, CComPtr<ID3D11InputLayout>> m_inputLayouts;
};