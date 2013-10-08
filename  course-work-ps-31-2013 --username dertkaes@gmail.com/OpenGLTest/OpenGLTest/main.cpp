#include <windows.h>
#include <GL\glut.h>
#include "3dObject.h"
#include "ObjectManager.h"
#include "Table.h"
#include "SkyBox.h"
#include "Camera.h"
#include "Selection.h"

CObjectManager objectManager;
IObject * selected;
CCamera camera;
CTable table(30.0f, 15.0f, "sand.bmp");
CSkyBox skybox(0.0, 0.0, 0.0, 60.0, 60.0, 60.0, "skybox");

void DrawSelectionBox(IObject * object);

void OnDrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.Update();
	skybox.Draw();
	table.Draw();
	objectManager.Draw();
	if(selected) DrawSelectionBox(selected);
	glutSwapBuffers();
}

void OnIdle()
{
	glutPostRedisplay();
}

void OnMouse(int button, int state, int x, int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON: //LMB
		if (state == GLUT_DOWN)
		{
			selected = SelectObject(x, y, objectManager);
		}
		else
		{
			//find intersection with z==0 plane and place model there
		}break;
	case 3://scroll up
		if (state == GLUT_UP)
		{
			camera.Scale(1.1); 
		}break;
	case 4://scroll down
		if (state == GLUT_UP)
		{
			camera.Scale(1.0 / 1.1);
		}break;
	}
}

void OnPassiveMouseMove(int x, int y)
{
	static int prevMouseX;
	static int prevMouseY;
	if(glutGetModifiers() == GLUT_ACTIVE_ALT)
	{
		camera.Rotate((double)(x - prevMouseX) / 10, (double)(prevMouseY - y) / 5);
	}
	prevMouseX = x;
	prevMouseY = y;
}

void OnKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 8://backspace
		{
			camera.Reset();
		}break;
	}
}

void OnSpecialKeyPress(int key, int x, int y)
{
   switch (key) 
   {
	case GLUT_KEY_LEFT:
		  {
			  camera.Translate(0.3, 0.0);
		  }break;
	 case GLUT_KEY_RIGHT:
		  {
			 camera.Translate(-0.3, 0.0);
		  }break;
	 case GLUT_KEY_DOWN:
		  {
			  camera.Translate(0.0, 0.3);
		  }break;
	 case GLUT_KEY_UP:
		  {
			  camera.Translate(0.0, -0.3);
		  }break;
   }
}

void OnReshape(int width, int height) 
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)width / (GLdouble)height;
	gluPerspective(60, aspect, 0.5, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE /*hPrevInstance*/,LPSTR /*lpCmdLine*/,int nCmdShow)
{
	selected = NULL;
	objectManager.AddObject(new C3DObject("CSM.obj", 0.0, 0.0, 0.0));
	objectManager.AddObject(new C3DObject("SpaceMarine.obj", -2.0, 0.0, 0.0));
	objectManager.AddObject(new C3DObject("SpaceMarine.obj", 2.0, 0.0, 0.0));
	int argc = 0;
	char* argv[] = {""};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("GLUT test");
	glutDisplayFunc(&OnDrawScene);
	glutIdleFunc(&OnIdle);
	glutReshapeFunc(&OnReshape);
	glutKeyboardFunc(&OnKeyboard);
	glutSpecialFunc(&OnSpecialKeyPress);
	glutMouseFunc(&OnMouse);
	glutPassiveMotionFunc(&OnPassiveMouseMove);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glutMainLoop();
	return 0;
}