#pragma once
#include <string>
#include "../view/IShaderManager.h"
#include <vector>

class CShaderManagerOpenGLES : public IShaderManager
{
public:
	CShaderManagerOpenGLES();
	std::unique_ptr<IShaderProgram> NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") override;
	void PushProgram(IShaderProgram const& program) const override;
	void PopProgram() const override;

	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, const float* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, const int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t totalSize, const unsigned int* values, bool perInstance = false) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const override;

	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const float* value) const override;
	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const int* value) const override;
	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const unsigned int* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, bool perInstance = false) const override;

	int GetVertexLocation() const { return m_positionLocation; }
	int GetNormalLocation() const { return m_normalsLocation; }
	int GetTexCoordLocation() const { return m_texCoordLocation; }
	void DoOnProgramChange(std::function<void()> const& handler);
private:
	void SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t count, const void* values, bool perInstance, unsigned int format) const;
	mutable std::vector<unsigned> m_programs;
	std::function<void()> m_onProgramChange;
	int m_positionLocation;
	int m_normalsLocation;
	int m_texCoordLocation;
};