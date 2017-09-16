namespace wargameEngine
{
class IScriptHandler;
class AsyncFileProvider;
class ThreadPool;

namespace model
{
class Model;
}
namespace view
{
class View;
class TranslationManager;
class ModelManager;
}
namespace ui
{
class IUIElement;
class IUITextHelper;
}

namespace controller
{
class Controller;

void RegisterModelFunctions(IScriptHandler & handler, model::Model & model);
void RegisterViewFunctions(IScriptHandler & handler, view::View & view, AsyncFileProvider& fileProvider);
void RegisterControllerFunctions(IScriptHandler & handler, Controller & controller, AsyncFileProvider & fileProvider, ThreadPool & threadPool);
void RegisterObject(IScriptHandler & handler, Controller & controller, model::Model & model, view::ModelManager & modelManager);
void RegisterUI(IScriptHandler & handler, ui::IUIElement * uiRoot, view::TranslationManager & transMan, ui::IUITextHelper& textHelper);
void RegisterViewport(IScriptHandler & handler, view::View & view);
}
}