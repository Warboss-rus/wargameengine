#include <string>

class CShaderManager
{
public:
	CShaderManager():m_program(-1) {}
	void NewProgram(std::string const& vertex = "", std::string const& fragment = "", std::string const& geometry = "");
	void BindProgram() const;
	void UnBindProgram() const;
	template<class T>
	void SetUniformValue(std::string const& uniform, T value) const;
	template<class T>
	void SetUniformValue(std::string const& uniform, int count, T value) const;
	template<class T>
	void SetUniformValue2(std::string const& uniform, int count, T value) const;
	template<class T>
	void SetUniformValue3(std::string const& uniform, int count, T value) const;
	template<class T>
	void SetUniformValue4(std::string const& uniform, int count, T value) const;
	template<class T>
	void SetUniformMatrix4(std::string const& uniform, int count, T value) const;
	//Automaticly enables the array
	template<class T>
	void SetVertexAttrib4(unsigned int index, T value) const;
	//Disables the array and sets the default value
	template<class T>
	void DisableVertexAttrib4(unsigned int index, T defaultValue) const;
private:
	unsigned int m_program;
};