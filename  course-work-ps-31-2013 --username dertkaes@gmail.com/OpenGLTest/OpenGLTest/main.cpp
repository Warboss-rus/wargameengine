#include <windows.h>

#include <memory>

#include "view\GameView.h"
#include "model\GameModel.h"
#include "controller\GameController.h"
#include "3dObject.h"
#include <time.h>

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE /*hPrevInstance*/,LPSTR /*lpCmdLine*/,int nCmdShow)
{
	std::weak_ptr<CGameModel> pModel = CGameModel::GetIntanse();
	std::weak_ptr<CGameController> pController = CGameController::GetIntanse();
	std::weak_ptr<CGameView> pView = CGameView::GetIntanse(); 
	srand(time(NULL));
//	CCommandManager commandManager(pView.lock().get());
//	commandManager.Execute(new CCreateObjectCommand());
	
	return 0;
}
