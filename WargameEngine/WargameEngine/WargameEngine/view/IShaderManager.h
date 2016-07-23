#pragma once
#include <string>

class IShaderManager
{
public:
	enum class eVertexAttribute
	{
		WEIGHT,
		WEIGHT_INDEX,
	};

	virtual ~IShaderManager() {}

	virtual void NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") = 0;
	virtual void BindProgram() const = 0;
	virtual void UnBindProgram() const = 0;

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

	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t totalSize, float* values) const= 0;
	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t totalSize, int* values) const = 0;
	virtual void SetVertexAttribute(eVertexAttribute attributeIndex, int elementSize, size_t totalSize, unsigned int* values) const = 0;

	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, float* defaultValue) const = 0;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, int* defaultValue) const = 0;
	virtual void DisableVertexAttribute(eVertexAttribute attributeIndex, int size, unsigned int* defaultValue) const = 0;
};