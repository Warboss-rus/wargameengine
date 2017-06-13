#pragma once
#include "../Typedefs.h"
#include <memory>
#include <string>

namespace wargameEngine
{
namespace view
{

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

	virtual std::unique_ptr<IShaderProgram> NewProgram(const Path& vertex = Path(), const Path& fragment = Path(), const Path& geometry = Path()) = 0;
	virtual std::unique_ptr<IShaderProgram> NewProgramSource(const std::string& vertex = "", const std::string& fragment = "", const std::string& geometry = "") = 0;
	virtual void PushProgram(IShaderProgram const& program) const = 0;
	virtual void PopProgram() const = 0;

	virtual void SetUniformValue(const std::string& uniform, int elementSize, size_t count, const float* value) const = 0;
	virtual void SetUniformValue(const std::string& uniform, int elementSize, size_t count, const int* value) const = 0;
	virtual void SetUniformValue(const std::string& uniform, int elementSize, size_t count, const unsigned int* value) const = 0;

	virtual void SetVertexAttribute(const std::string& attribute, int elementSize, size_t count, const float* values, bool perInstance = false) const = 0;
	virtual void SetVertexAttribute(const std::string& attribute, int elementSize, size_t count, const int* values, bool perInstance = false) const = 0;
	virtual void SetVertexAttribute(const std::string& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance = false) const = 0;

	virtual void DisableVertexAttribute(const std::string& attribute, int size, const float* defaultValue) const = 0;
	virtual void DisableVertexAttribute(const std::string& attribute, int size, const int* defaultValue) const = 0;
	virtual void DisableVertexAttribute(const std::string& attribute, int size, const unsigned int* defaultValue) const = 0;

	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(size_t size, const void* value) const = 0;

	enum class Format
	{
		Float32,
		SInt32,
		UInt32
	};
	virtual void SetVertexAttribute(const std::string& attribute, IVertexAttribCache const& cache, int elementSize, size_t count, Format type, bool perInstance = false, size_t offset = 0) const = 0;

	virtual bool NeedsMVPMatrix() const = 0;
	virtual void SetMatrices(const float* model = nullptr, const float* view = nullptr, const float* projection = nullptr, const float* mvp = nullptr, size_t multiviewCount = 1) = 0;
};
}
}