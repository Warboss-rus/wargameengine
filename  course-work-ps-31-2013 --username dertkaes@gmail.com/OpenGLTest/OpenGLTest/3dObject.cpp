#include "3dObject.h"
#include <GL\glut.h>

C3DObject::C3DObject()
{
	//1
	a.push_back(0.0);
	a.push_back(0.0);
	a.push_back(0.0);
	//2
	a.push_back(1.0);
	a.push_back(0.0);
	a.push_back(0.0);
	//3
	a.push_back(1.0);
	a.push_back(1.0);
	a.push_back(0.0);
	/*//4
	a.push_back(0.0);
	a.push_back(0.0);
	a.push_back(0.0);
	//5
	a.push_back(-1.0);
	a.push_back(0.0);
	a.push_back(0.0);
	//6
	a.push_back(-1.0);
	a.push_back(1.0);
	a.push_back(0.0);*/
	m_count = a.size() / 3;
}

void C3DObject::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(m_count, GL_DOUBLE, sizeof(GLdouble) * 3, &a[0]);
	glDrawArrays(GL_TRIANGLES, 0, m_count);
	glDisableClientState(GL_VERTEX_ARRAY);
}