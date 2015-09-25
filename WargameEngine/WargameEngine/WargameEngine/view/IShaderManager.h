#pragma once
#include <string>

class IShaderManager
{
public:
	enum eUniformIndex
	{
		WEIGHT = 16,
		WEIGHT_INDEX = 17,
	};

	virtual ~IShaderManager() {}

	virtual void NewProgram(std::string const& vertex = "", std::string const& fragment = "", std::string const& geometry = "") = 0;
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

	virtual void SetUniformMatrix4(std::string const& uniform, int count, float* value) const = 0;

	virtual void SetVertexAttribute(eUniformIndex attributeIndex, int size, float* values) const= 0;
	virtual void SetVertexAttribute(eUniformIndex attributeIndex, int size, int* values) const = 0;
	virtual void SetVertexAttribute(eUniformIndex attributeIndex, int size, unsigned int* values) const = 0;

	virtual void DisableVertexAttribute(eUniformIndex attributeIndex, int size, float* defaultValue) const = 0;
	virtual void DisableVertexAttribute(eUniformIndex attributeIndex, int size, int* defaultValue) const = 0;
	virtual void DisableVertexAttribute(eUniformIndex attributeIndex, int size, unsigned int* defaultValue) const = 0;
};