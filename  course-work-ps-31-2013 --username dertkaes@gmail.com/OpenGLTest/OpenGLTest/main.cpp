#include <windows.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include "view\GameView.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE /*hPrevInstance*/,LPSTR /*lpCmdLine*/,int nCmdShow)
{
	CGameView::GetIntanse();
	return 0;
}
