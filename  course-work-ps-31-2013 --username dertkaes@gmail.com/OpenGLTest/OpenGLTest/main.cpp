#include <windows.h>
#include "view\GameView.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE /*hPrevInstance*/,LPSTR /*lpCmdLine*/,int nCmdShow)
{
	CGameView::GetIntanse();
	return 0;
}
