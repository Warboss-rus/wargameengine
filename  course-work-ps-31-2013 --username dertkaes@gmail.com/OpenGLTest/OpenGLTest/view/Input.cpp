#include "Input.h"
#include <GL\glut.h>
#include "GameView.h"

CInput::CInput(void)
{
	glutKeyboardFunc(&OnKeyboard);
	glutSpecialFunc(&OnSpecialKeyPress);
	glutMouseFunc(&OnMouse);
	glutPassiveMotionFunc(&OnPassiveMouseMove);
}


CInput::~CInput(void)
{
}

void CInput::OnMouse(int button, int state, int x, int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON: //LMB
		if (state == GLUT_DOWN)
		{
			//selected = SelectObject(x, y, objectManager);
		}
		else
		{
			//find intersection with z==0 plane and place model there
		}break;
	case 3://scroll up
		if (state == GLUT_UP)
		{
			CGameView::GetIntanse().lock()->CameraZoomIn();
		}break;
	case 4://scroll down
		if (state == GLUT_UP)
		{
			CGameView::GetIntanse().lock()->CameraZoomOut();
		}break;
	}
}

void CInput::OnKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 8://backspace
		{
			CGameView::GetIntanse().lock()->CameraReset();
		}break;
	}
}

void CInput::OnSpecialKeyPress(int key, int x, int y)
{
   switch (key) 
   {
	case GLUT_KEY_LEFT:
		  {
			  CGameView::GetIntanse().lock()->CameraTranslateLeft();
//			  camera.Translate(0.3, 0.0);
		  }break;
	 case GLUT_KEY_RIGHT:
		  {
			 CGameView::GetIntanse().lock()->CameraTranslateRight();
//			 camera.Translate(-0.3, 0.0);
		  }break;
	 case GLUT_KEY_DOWN:
		  {
			  CGameView::GetIntanse().lock()->CameraTranslateDown();
//			  camera.Translate(0.0, 0.3);
		  }break;
	 case GLUT_KEY_UP:
		  {
			  CGameView::GetIntanse().lock()->CameraTranslateUp();
	//		  camera.Translate(0.0, -0.3);
		  }break;
   }
}

void CInput::OnPassiveMouseMove(int x, int y)
{
	static int prevMouseX;
	static int prevMouseY;
	if(glutGetModifiers() == GLUT_ACTIVE_ALT)
	{
//		camera.Rotate((double)(x - prevMouseX) / 10, (double)(prevMouseY - y) / 5);
	}
	prevMouseX = x;
	prevMouseY = y;
}