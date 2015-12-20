class IScriptHandler;
class CGameController;
class CGameModel;
class CGameView;

void RegisterFunctions(IScriptHandler & handler, CGameController & controller);
void RegisterObject(IScriptHandler & handler, CGameController & controller, CGameModel & model);
void RegisterUI(IScriptHandler & handler, CGameView & view);