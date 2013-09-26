#include <windows.h>
#include <GL\glut.h>
#include "3dObject.h"

void OnDrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glutWireSphere(0.6, 20, 20);
	C3DObject obj;
	obj.Draw();
	glutSwapBuffers();
}

void OnMouse(int a, int b, int x, int y)
{

}

void OnKeyPress(unsigned char key, int x, int y)
{
   switch (key) {
      case 27: /* escape */
         exit(0);
         break;
	  case 'a':
		  {
			  gluLookAt(-0.1, 0.0, -1.0, -0.1, 0.0, 0.0, 0.0, 1.0, 0.0);
			  glutPostRedisplay();
		  }break;
	  case 'd':
		  {
			  gluLookAt(0.1, 0.0, -1.0, 0.1, 0.0, 0.0, 0.0, 1.0, 0.0);
			  glutPostRedisplay();
		  }break;
	  case 's':
		  {
			  gluLookAt(0.0, 0.1, -1.0, 0.0, 0.1, 0.0, 0.0, 1.0, 0.0);
			  glutPostRedisplay();
		  }break;
	  case 'w':
		  {
			  gluLookAt(0.0, -0.1, -1.0, 0.0, -0.1, 0.0, 0.0, 1.0, 0.0);
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
	glutMainLoop();
	return 0;
}