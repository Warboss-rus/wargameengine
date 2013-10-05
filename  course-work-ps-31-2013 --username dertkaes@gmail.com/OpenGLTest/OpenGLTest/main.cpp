#include <windows.h>
#include <GL\glut.h>
#include "3dObject.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "Table.h"
#include "SkyBox.h"

CObjectManager objectManager;
CModelManager modelManager;
IObject * selected;
//CTable table(6.0f, 3.0f, "sand.bmp");
//CSkyBox skybox(0.0, 0.0, 0.0, 100.0, 100.0, 100.0, "texture\\skybox");

void DrawSelectionBox(IObject * object);

void OnDrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//skybox.Draw();
	//table.Draw();
	objectManager.Draw();
	if(selected) DrawSelectionBox(selected);
	glutSwapBuffers();
}

void DrawSelectionBox(IObject * object)
{
	glPushMatrix();
	glTranslated(object->GetX(), object->GetY(), object->GetZ());
	glRotated(object->GetRotation(), 0.0, 0.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3d(0.0, 0.0, 255.0);
	const double * box = object->GetBounding();
	glBegin(GL_QUADS);
	//Left
	glVertex3d(box[3], box[1], box[2]);
	glVertex3d(box[3], box[4], box[2]);
	glVertex3d(box[3], box[4], box[5]);
	glVertex3d(box[3], box[1], box[5]);
	//Right
	glVertex3d(box[0], box[1], box[2]);
	glVertex3d(box[0], box[4], box[2]);
	glVertex3d(box[0], box[4], box[5]);
	glVertex3d(box[0], box[1], box[5]);
	//Front
	glVertex3d(box[0], box[1], box[2]);
	glVertex3d(box[3], box[1], box[2]);
	glVertex3d(box[3], box[1], box[5]);
	glVertex3d(box[0], box[1], box[5]);
	//Back
	glVertex3d(box[0], box[4], box[2]);
	glVertex3d(box[3], box[4], box[2]);
	glVertex3d(box[3], box[4], box[5]);
	glVertex3d(box[0], box[4], box[5]);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
}

IObject* SelectObject(int x, int y)
{
	//Get model, projection and viewport matrices
	double matModelView[16], matProjection[16]; 
	int viewport[4]; 
	glGetDoublev( GL_MODELVIEW_MATRIX, matModelView ); 
	glGetDoublev( GL_PROJECTION_MATRIX, matProjection ); 
	glGetIntegerv( GL_VIEWPORT, viewport ); 
	//Set OpenGL Windows coordinates
	double winX = (double)x; 
	double winY = viewport[3] - (double)y;
	double startx, starty, startz, endx, endy, endz;
	//Cast a ray from eye to mouse cursor;
	gluUnProject(winX, winY, 0.0, matModelView, matProjection, 
             viewport, &startx, &starty, &startz); 
	gluUnProject(winX, winY, 1.0, matModelView, matProjection, 
             viewport, &endx, &endy, &endz);
	return objectManager.GetNearestObjectByVector(startx, starty, startz, endx, endy, endz);
}

void OnMouse(int button, int state, int x, int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON: //LMB
		if (state == GLUT_DOWN)
		{
			selected = SelectObject(x, y);
		}
		else
		{
			//find intersection with z==0 plane and place model there
		}break;
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
	selected = new C3DObject(modelManager.GetModel("SpaceMarine2.obj"), 0.0, 0.0, 0.0);
	objectManager.AddObject(selected);
	objectManager.AddObject(new C3DObject(modelManager.GetModel("SpaceMarine.obj"), -2.0, 0.0, 0.0));
	objectManager.AddObject(new C3DObject(modelManager.GetModel("SpaceMarine.obj"), 2.0, 0.0, 0.0));
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