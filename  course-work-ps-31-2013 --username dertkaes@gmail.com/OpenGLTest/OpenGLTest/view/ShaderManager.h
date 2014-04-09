#include <string>

class CShaderManager
{
public:
	CShaderManager():m_program(-1) {}
	void NewProgram(std::string const& vertex = "", std::string const& fragment = "", std::string const& geometry = "");
	void BindProgram() const;
	void UnBindProgram() const;
	template<class T>
	void SetUniformValue(std::string const& uniform, T value);
private:
	unsigned int m_program;
};