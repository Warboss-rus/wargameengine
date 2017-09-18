#pragma once
#include <string>
#include "../view/IShaderManager.h"
#include <vector>

class CShaderManagerLegacyGL : public wargameEngine::view::IShaderManager
{
public:
	CShaderManagerLegacyGL();
	std::unique_ptr<wargameEngine::view::IShaderProgram> NewProgram(const wargameEngine::Path& vertex, const wargameEngine::Path& fragment, const wargameEngine::Path& geometry) override;
	std::unique_ptr<wargameEngine::view::IShaderProgram> NewProgramSource(std::string const& vertex = "", std::string const& fragment = "", std::string const& geometry = "") override;
	void PushProgram(wargameEngine::view::IShaderProgram const& shaderProgram) const override;
	void PopProgram() const override;

	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const float* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const int* value) const override;
	virtual void SetUniformValue(std::string const& uniform, int elementSize, size_t count, const unsigned int* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const float* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const int* values, bool perInstance = false) const override;
	virtual void SetVertexAttribute(std::string const& attribute, int elementSize, size_t count, const unsigned int* values, bool perInstance = false) const override;

	virtual void DisableVertexAttribute(std::string const& attribute, int size, const float* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const int* defaultValue) const override;
	virtual void DisableVertexAttribute(std::string const& attribute, int size, const unsigned int* defaultValue) const override;

	virtual std::unique_ptr<wargameEngine::view::IVertexAttribCache> CreateVertexAttribCache(size_t size, const void* value) const override;

	virtual void SetVertexAttribute(std::string const& attribute, wargameEngine::view::IVertexAttribCache const& cache, int elementSize, size_t count, Format type, bool perInstance = false, size_t offset = 0) const override;

	virtual bool NeedsMVPMatrix() const override;
	virtual void SetMatrices(const float* model = nullptr, const float* view = nullptr, const float* projection = nullptr, const float* mvp = nullptr, size_t multiviewCount = 1);
private:
	void SetVertexAttributeImpl(std::string const& attribute, int elementSize, size_t count, const void* values, bool perInstance, unsigned int format) const;
	void NewProgramImpl(unsigned program, unsigned vertexShader, unsigned framgentShader);
	mutable std::vector<unsigned int> m_programs;
};