#pragma once
#include <string>
#include "../view/IShaderManager.h"
#include <vector>

class CShaderManagerOpenGL : public IShaderManager
{
public:
	CShaderManagerOpenGL();
	std::unique_ptr<IShaderProgram> NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") override;
	void PushProgram(IShaderProgram const& shaderProgram) const override;
	void PopProgram() const override;

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
private:
	mutable std::vector<unsigned int> m_programs;
};