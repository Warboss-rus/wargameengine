#include <windows.h>
#include <GL\glut.h>
#include "3dModel.h"

C3DModel model("teapot.obj");

void OnDrawScene()
{
	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw();
	glPopMatrix();
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

void OnKeyPress(unsigned char key, int x, int y)
{
   switch (key) {
      case 27: /* escape */
         exit(0);
         break;
	  case 'a':
		  {
			  glTranslated(0.1, 0.0, 0.0);
			  glutPostRedisplay();
		  }break;
	  case 'd':
		  {
			  glTranslated(-0.1, 0.0, 0.0);
			  glutPostRedisplay();
		  }break;
	  case 's':
		  {
			  glTranslated(0.0, 0.1, 0.0);
			  glutPostRedisplay();
		  }break;
	  case 'w':
		  {
			  glTranslated(0.0, -0.1, 0.0);
			  glutPostRedisplay();
		  }break;
   }
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
	glutKeyboardFunc(&OnKeyPress);
	glutMouseFunc(&OnMouse);
	glEnable(GL_NORMALIZE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glutMainLoop();
	return 0;
}