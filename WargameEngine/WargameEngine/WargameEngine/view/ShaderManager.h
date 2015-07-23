#include <string>
#pragma once

class CShaderManager
{
public:
	enum eUniformIndex
	{
		WEIGHT = 16,
		WEIGHT_INDEX = 17,
	};
	CShaderManager():m_program(0) {}
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
	template<class T>
	//Automaticly enables vertexAttribs
	void SetVertexAttrib4(unsigned int index, T value) const;
	template<class T>
	//Disables the vertexAttribs and sets the default value
	void DisableVertexAttrib4(unsigned int index, T defaultValue) const;
private:
	unsigned int m_program;
};