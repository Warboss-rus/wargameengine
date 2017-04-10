#pragma once
#include <string>
#include <memory>

class IShaderProgram
{
public:
	virtual ~IShaderProgram() {}
};

class IVertexAttribCache
{
public:
	virtual ~IVertexAttribCache() {}
};

class IShaderManager
{
public:
	virtual ~IShaderManager() {}

	virtual std::unique_ptr<IShaderProgram> NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") = 0;
	virtual std::unique_ptr<IShaderProgram> NewProgramSource(std::string const& vertex = "", std::string const& fragment = "", std::string const& geometry = "") = 0;
	virtual void PushProgram(IShaderProgram const& program) const = 0;
	virtual void PopProgram() const = 0;

	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const = 0;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const = 0;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const = 0;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance = false) const = 0;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance = false) const = 0;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance = false) const = 0;

	virtual void DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const = 0;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const = 0;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const = 0;

	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(size_t size, const void* value) const = 0;

	enum class TYPE
	{
		FLOAT32,
		SINT32,
		UINT32
	};
	virtual void SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, int elementSize, size_t count, TYPE type, bool perInstance = false, size_t offset = 0) const = 0;
};