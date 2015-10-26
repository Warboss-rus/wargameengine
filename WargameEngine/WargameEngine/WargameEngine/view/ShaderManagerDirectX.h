#pragma once
#include "IShaderManager.h"
#include <d3d11.h>
#include <atlcomcli.h>

class CShaderManagerDirectX : public IShaderManager
{
public:
	CShaderManagerDirectX(ID3D11Device *dev, ID3D11DeviceContext *devcon, IShaderManager * parent = nullptr);

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
	virtual void SetUniformMatrix4(std::string const& uniform, int count, float* value) const override;

	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int size, float* values) const override;
	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int size, int* values) const override;
	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int size, unsigned int* values) const override;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, float* defaultValue) const override;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, int* defaultValue) const override;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, unsigned int* defaultValue) const override;
private:
	ID3D11Device *m_dev;
	ID3D11DeviceContext *m_devcon;
	IShaderManager * m_parent;
	CComPtr<ID3D11VertexShader> pVS;    // the vertex shader
	CComPtr<ID3D11PixelShader> pPS;     // the pixel shader
	CComPtr<ID3D11GeometryShader> pGS;     // the geometry shader
};