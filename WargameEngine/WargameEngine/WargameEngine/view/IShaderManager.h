#pragma once
#include <string>
#include <memory>

class IShaderProgram
{
public:
	virtual ~IShaderProgram() {}
};

class IShaderManager
{
public:
	virtual ~IShaderManager() {}

	virtual std::unique_ptr<IShaderProgram> NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") = 0;
	virtual void PushProgram(IShaderProgram const& program) const = 0;
	virtual void PopProgram() const = 0;

	virtual void SetUniformValue(std::string const& uniform, int count, const float* value) const = 0;
	virtual void SetUniformValue(std::string const& uniform, int count, const int* value) const = 0;
	virtual void SetUniformValue(std::string const& uniform, int count, const unsigned int* value) const = 0;
	virtual void SetUniformValue2(std::string const& uniform, int count, const float* value) const = 0;
	virtual void SetUniformValue2(std::string const& uniform, int count, const int* value) const = 0;
	virtual void SetUniformValue2(std::string const& uniform, int count, const unsigned int* value) const = 0;
	virtual void SetUniformValue3(std::string const& uniform, int count, const float* value) const = 0;
	virtual void SetUniformValue3(std::string const& uniform, int count, const int* value) const = 0;
	virtual void SetUniformValue3(std::string const& uniform, int count, const unsigned int* value) const = 0;
	virtual void SetUniformValue4(std::string const& uniform, int count, const float* value) const = 0;
	virtual void SetUniformValue4(std::string const& uniform, int count, const int* value) const = 0;
	virtual void SetUniformValue4(std::string const& uniform, int count, const unsigned int* value) const = 0;

	virtual void SetUniformMatrix4(std::string const& uniform, size_t count, float* value) const = 0;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, float* values) const= 0;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, int* values) const = 0;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, unsigned int* values) const = 0;

	virtual void SetPerInstanceVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, float* values) const = 0;

	virtual void DisableVertexAttribute(std::string const& attribute, int size, float* defaultValue) const = 0;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, int* defaultValue) const = 0;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, unsigned int* defaultValue) const = 0;
};