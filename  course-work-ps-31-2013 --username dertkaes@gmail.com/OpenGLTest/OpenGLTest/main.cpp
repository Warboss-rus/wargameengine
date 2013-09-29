#include <windows.h>
#include <GL\glut.h>
#include "3dModel.h"
#include "Table.h"

C3DModel model("SpaceMarine.obj");
CTable table(2.0, 1.0);

void OnDrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//table.Draw();
	model.Draw();
	glutSwapBuffers();
}

void OnMouse(int button, int state, int x, int y)
{
	switch(button)
	{
	case 3://scroll up
		if (state == GLUT_UP)
		{
			 glScaled(1.1, 1.1, 1.1);
			 glutPostRedisplay();
		}break;
	case 4://scroll down
		if (state == GLUT_UP)
		{
			glScaled(1.0 / 1.1, 1.0 / 1.1, 1.0 / 1.1);
			glutPostRedisplay();
		}break;
	}
}

void OnSpecialKeyPress(int key, int x, int y)
{
   switch (key) 
   {
	case GLUT_KEY_LEFT:
		  {
			  glTranslated(0.1, 0.0, 0.0);
			  glutPostRedisplay();
		  }break;
	 case GLUT_KEY_RIGHT:
		  {
			  glTranslated(-0.1, 0.0, 0.0);
			  glutPostRedisplay();
		  }break;
	 case GLUT_KEY_DOWN:
		  {
			  glTranslated(0.0, 0.1, 0.0);
			  glutPostRedisplay();
		  }break;
	 case GLUT_KEY_UP:
		  {
			  glTranslated(0.0, -0.1, 0.0);
			  glutPostRedisplay();
		  }break;
   }
}

void OnReshape(int width, int height) 
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//change view angle here
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE /*hPrevInstance*/,LPSTR /*lpCmdLine*/,int nCmdShow)
{
	int argc = 0;
	char* argv[] = {""};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("GLUT test");
	glutDisplayFunc(&OnDrawScene);
	glutReshapeFunc(&OnReshape);
	glutSpecialFunc(&OnSpecialKeyPress);
	glutMouseFunc(&OnMouse);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glutMainLoop();
	return 0;
}