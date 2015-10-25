#pragma once
#include <string>
#include "IShaderManager.h"

class CShaderManagerOpenGL : public IShaderManager
{
public:
	
	CShaderManagerOpenGL():m_program(0) {}
	void NewProgram(std::string const& vertex = "", std::string const& fragment = "", std::string const& geometry = "") override;
	void BindProgram() const override;
	void UnBindProgram() const override;

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
	virtual void SetUniformMatrix4(std::string const& uniform, int count, float* values) const override;

	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int size, float* values) const override;
	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int size, int* values) const override;
	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int size, unsigned int* values) const override;

	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, float* defaultValue) const override;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, int* defaultValue) const override;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, unsigned int* defaultValue) const override;


private:
	unsigned int m_program;
};