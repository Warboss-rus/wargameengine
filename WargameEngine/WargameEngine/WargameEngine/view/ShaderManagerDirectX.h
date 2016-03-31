#pragma once
#include "IShaderManager.h"
#include <d3d11.h>
#include <atlcomcli.h>
#include <d3d11shader.h>
#include <vector>

class CDirectXRenderer;
struct sLightSource;

class CShaderManagerDirectX : public IShaderManager
{
public:
	CShaderManagerDirectX(ID3D11Device *dev, CDirectXRenderer * render = nullptr);

	virtual void NewProgram(std::string const& vertex = "", std::string const& fragment = "", std::string const& geometry = "") override;
	virtual void BindProgram() const override;
	virtual void UnBindProgram() const override;

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

	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t totalSize, float* values) const override;
	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t totalSize, int* values) const override;
	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t totalSize, unsigned int* values) const override;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, float* defaultValue) const override;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, int* defaultValue) const override;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, unsigned int* defaultValue) const override;

	void SetInputLayout(DXGI_FORMAT vertexFormat, DXGI_FORMAT texCoordFormat, DXGI_FORMAT normalFormat);
	void SetMatrices(float * modelView, float * projection);
	void SetColor(const float * color);
	void SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess);
	void SetLight(size_t index, sLightSource & lightSource);
private:
	void CreateConstantBuffer(unsigned int size, ID3D11Buffer ** m_constantBuffer) const;
	unsigned int GetVariableOffset(std::string const& buffer, std::string const& name, unsigned int * size = nullptr) const;
	void CopyConstantBufferData(unsigned int begin, const void * data, unsigned int size) const;
	void CreateBuffer(ID3D11Buffer ** bufferPtr, unsigned int size) const;
	void CopyBufferData(ID3D11Buffer * buffer, const void * data, unsigned int size) const;

	CComPtr<ID3D11Device> m_dev;
	CDirectXRenderer * m_render;
	CComPtr<ID3D11VertexShader> pVS;    // the vertex shader
	CComPtr<ID3D11PixelShader> pPS;     // the pixel shader
	CComPtr<ID3D11GeometryShader> pGS;     // the geometry shader
	CComPtr<ID3D10Blob> m_VS;
	CComPtr<ID3D11ShaderReflection> m_reflection;
	mutable std::vector<unsigned char> m_constantBufferData;
};