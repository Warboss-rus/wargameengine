#include <windows.h>
#include <GL\glut.h>
#include "3dObject.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "Table.h"
#include "SkyBox.h"

CObjectManager objectManager;
CModelManager modelManager;
//CTable table(6.0f, 3.0f, "sand.bmp");
//CSkyBox skybox(0.0, 0.0, 0.0, 100.0, 100.0, 100.0, "texture\\skybox");

bool lineDrawing;

void OnDrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//table.Draw();
	//skybox.Draw();
	objectManager.Draw();
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

void OnKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'a':
		{
			glRotated(1.0, 0.0, 1.0, 0.0);
			glutPostRedisplay();
		}break;
	case 'd':
		{
			glRotated(-1.0, 0.0, 1.0, 0.0);
			glutPostRedisplay();
		}break;
	case 'w':
		{
			glRotated(1.0, 0.0, 0.0, 1.0);
			glutPostRedisplay();
		}break;
	case 's':
		{
			glRotated(-1.0, 0.0, 0.0, 1.0);
			glutPostRedisplay();
		}break;
	case 8://backspace
		{
			glLoadIdentity();
			glutPostRedisplay();
		}break;
	case 32: //space
		{
			glPolygonMode(GL_FRONT_AND_BACK, lineDrawing?GL_FILL:GL_LINE);
			lineDrawing = !lineDrawing;
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
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE /*hPrevInstance*/,LPSTR /*lpCmdLine*/,int nCmdShow)
{
	objectManager.AddObject(new C3DObject(modelManager.GetModel("SpaceMarine2.obj"), 0.0, 0.0, 0.0));
	objectManager.AddObject(new C3DObject(modelManager.GetModel("SpaceMarine.obj"), -2.0, 0.0, 0.0));
	objectManager.AddObject(new C3DObject(modelManager.GetModel("SpaceMarine.obj"), 2.0, 0.0, 0.0));
	lineDrawing = false;
	int argc = 0;
	char* argv[] = {""};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("GLUT test");
	glutDisplayFunc(&OnDrawScene);
	glutReshapeFunc(&OnReshape);
	glutKeyboardFunc(&OnKeyboard);
	glutSpecialFunc(&OnSpecialKeyPress);
	glutMouseFunc(&OnMouse);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glutMainLoop();
	return 0;
}