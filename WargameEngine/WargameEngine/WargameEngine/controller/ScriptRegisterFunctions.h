class IScriptHandler;
class CGameController;
class CGameModel;
class CGameView;
class IUIElement;
class CTranslationManager;
class CModelManager;
class CAsyncFileProvider;
class ThreadPool;

void RegisterModelFunctions(IScriptHandler & handler, CGameModel & model);
void RegisterViewFunctions(IScriptHandler & handler, CGameView & view);
void RegisterControllerFunctions(IScriptHandler & handler, CGameController & controller, CAsyncFileProvider & fileProvider, ThreadPool & threadPool);
void RegisterObject(IScriptHandler & handler, CGameController & controller, CGameModel & model, CModelManager & modelManager);
void RegisterUI(IScriptHandler & handler, IUIElement * uiRoot, CTranslationManager & transMan);
void RegisterViewport(IScriptHandler & handler, CGameView & view);