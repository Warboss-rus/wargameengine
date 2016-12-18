#pragma once
#include "../view/IShaderManager.h"
#include <vector>
#include <functional>
#include <map>

class CShaderManagerOpenGL : public IShaderManager
{
public:
	static constexpr char* VERTEX_ATTRIB_NAME = "Position";
	static constexpr char* NORMAL_ATTRIB_NAME = "Normal";
	static constexpr char* TEXCOORD_ATTRIB_NAME = "TexCoord";

	CShaderManagerOpenGL();
	~CShaderManagerOpenGL();
	std::unique_ptr<IShaderProgram> NewProgram(std::wstring const& vertex = L"", std::wstring const& fragment = L"", std::wstring const& geometry = L"") override;
	void PushProgram(IShaderProgram const& shaderProgram) const override;
	void PopProgram() const override;

	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, IVertexAttribCache const& cache, bool perInstance = false) const override;
	void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const double* values, bool perInstance = false) const;

	virtual void DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const override;

	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const float* value) const override;
	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const int* value) const override;
	virtual std::unique_ptr<IVertexAttribCache> CreateVertexAttribCache(int elementSize, size_t count, const unsigned int* value) const override;

	void DoOnProgramChange(std::function<void()> const& handler);
private:
	void SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t count, const void* values, bool perInstance, unsigned int format) const;
	mutable std::vector<unsigned int> m_programs;
	std::function<void()> m_onProgramChange;
	mutable std::map<std::string, unsigned> m_vertexAttribBuffers;
};