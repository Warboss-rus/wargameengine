#include "GlutInitializer.h"
#include <GL\glut.h>

using namespace std;

void GlutInitialize(void)
{
	int argc = 0;
	char* argv[] = {""};
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(600, 600);
	glutCreateWindow("GLUT test");

	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glutMainLoop();
}
