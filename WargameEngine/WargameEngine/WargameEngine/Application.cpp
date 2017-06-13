#include "Application.h"
#include "IPathfinding.h"
#include "IPhysicsEngine.h"
#include "IScriptHandler.h"
#include "controller/Controller.h"
#include "model/Model.h"
#include "view/IWindow.h"
#include "view/IImageReader.h"
#include "view/IModelReader.h"
#include "view/ISoundPlayer.h"
#include "view/ITextWriter.h"
#include "view/View.h"

namespace wargameEngine
{
Application::Application(Context&& context)
	: m_context(std::move(context))
	, m_asyncFileProvider(m_threadPool)
	, m_boundingBoxManager(m_asyncFileProvider)
{
	m_view = std::make_unique<view::View>(*m_context.window, *m_context.soundPlayer, *m_context.textWriter, m_threadPool, m_asyncFileProvider,
		m_context.imageReaders, m_context.modelReaders, m_boundingBoxManager);
}

Application::~Application()
{
}

void Application::Run(Module&& module)
{
	m_module = std::move(module);
	m_asyncFileProvider.SetModule(m_module);
	m_model = std::make_unique<model::Model>();
	m_controller = std::make_unique<controller::Controller>(*m_model, *m_context.scriptHandler, *m_context.physicsEngine, *m_context.pathFinder, m_boundingBoxManager);
	m_view->Init(*m_model, *m_controller);
	m_controller->Init(*m_view, m_context.socketFactory, m_asyncFileProvider.GetScriptAbsolutePath(m_module.script), m_asyncFileProvider);

	m_context.scriptHandler->RegisterFunction(L"LoadModule", [this](IArguments const& args){
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (module file)");
		Path module = m_asyncFileProvider.GetAbsolutePath(args.GetPath(1));
		m_threadPool.CancelAll();
		m_threadPool.QueueCallback([this, module]() {
			Run(module);
		});
		return nullptr;
	});

	if (!m_mainLoopStarted)
	{
		m_mainLoopStarted = true;
		m_context.window->LaunchMainLoop();
	}
}

void Application::Run(const Path& modulePath)
{
	Run(Module(modulePath));
}

wargameEngine::view::View& Application::GetView()
{
	return *m_view;
}

}