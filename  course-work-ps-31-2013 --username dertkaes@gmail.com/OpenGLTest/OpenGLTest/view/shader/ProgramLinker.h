#pragma once
#include <vector>
#include "../gl.h"

class CProgramLinker
{
public:
	CProgramLinker(void);
	~CProgramLinker(void);
	void LinkProgram(GLuint program);
	void CheckStatus();
private:
	std::vector<GLuint> m_programs;
};
