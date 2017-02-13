#pragma once
#include "../view/IShaderManager.h"
#include <vector>
#include <functional>
#include <map>

class CShaderManagerOpenGLES : public IShaderManager
{
public:
	~CShaderManagerOpenGLES();
	std::unique_ptr<IShaderProgram> NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") override;
	void PushProgram(IShaderProgram const& program) const override;
	void PopProgram() const override;

	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, bool perInstance = false, size_t offset = 0) const override;

	virtual void DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const override;

	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const float* value) const override;
	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const int* value) const override;
	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const unsigned int* value) const override;

	void DoOnProgramChange(std::function<void()> const& handler);
private:
	struct ShaderProgramCache
	{
		std::map<std::string, int> attribLocations;
		std::map<std::string, int> uniformLocations;
		std::map<std::string, bool> attribState;
	};

	void SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t count, const void* values, bool perInstance, unsigned int format) const;
	ShaderProgramCache& GetProgramCache() const;
	int GetUniformLocation(std::string const& uniform) const;
		
	mutable std::vector<unsigned int> m_programs;
	mutable unsigned int m_activeProgram;
	std::function<void()> m_onProgramChange;
	mutable std::map<std::string, unsigned> m_vertexAttribBuffers;
	
	mutable std::map<unsigned, ShaderProgramCache> m_shaderProgramCache;
};