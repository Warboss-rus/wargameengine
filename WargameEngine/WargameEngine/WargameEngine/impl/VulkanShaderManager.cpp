#include "VulkanShaderManager.h"



std::unique_ptr<IShaderProgram> CVulkanShaderManager::NewProgram(std::wstring const& vertex /*= L""*/, std::wstring const& fragment /*= L""*/, std::wstring const& geometry /*= L""*/)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::PushProgram(IShaderProgram const& program) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::PopProgram() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance /*= false*/) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance /*= false*/) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance /*= false*/) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, bool perInstance /*= false*/) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

void CVulkanShaderManager::DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(int elementSize, size_t count, const float* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(int elementSize, size_t count, const int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}

std::unique_ptr<IVertexAttribCache> CVulkanShaderManager::CreateVertexAttribCache(int elementSize, size_t count, const unsigned int* value) const
{
	throw std::logic_error("The method or operation is not implemented.");
}
