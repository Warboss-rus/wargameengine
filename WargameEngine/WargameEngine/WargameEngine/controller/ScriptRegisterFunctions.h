class IScriptHandler;
class CGameController;
class CGameModel;
class CGameView;
class IUIElement;
class CTranslationManager;
class CModelManager;

void RegisterFunctions(IScriptHandler & handler, CGameController & controller);
void RegisterObject(IScriptHandler & handler, CGameController & controller, CGameModel & model, CModelManager & modelManager);
void RegisterUI(IScriptHandler & handler, IUIElement * uiRoot, CTranslationManager & transMan);