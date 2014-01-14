#pragma once
#include <vector>

class CShaderCompiler
{
public:
	CShaderCompiler(void);
	~CShaderCompiler(void);
	void CompileShader(unsigned int shader);
	void CheckStatus();
private:
	std::vector<unsigned int> m_shaders;
};
