#pragma once
#include "AsyncFileProvider.h"
#include "Module.h"
#include "ThreadPool.h"
#include <functional>
#include <memory>
#include <vector>
#include "model\BoundingBoxManager.h"

namespace wargameEngine
{
class IScriptHandler;
class INetSocket;
class IPhysicsEngine;
class IPathfinding;
class ITextRasterizer;

namespace model
{
class Model;
}
namespace controller
{
class Controller;
}
namespace view
{
class View;
class IWindow;
class ISoundPlayer;
class IImageReader;
class IModelReader;
}

struct Context
{
	std::unique_ptr<view::IWindow> window;
	std::unique_ptr<view::ISoundPlayer> soundPlayer;
	std::unique_ptr<ITextRasterizer> textRasterizer;
	std::unique_ptr<IPhysicsEngine> physicsEngine;
	std::unique_ptr<IScriptHandler> scriptHandler;
	std::unique_ptr<IPathfinding> pathFinder;
	std::function<std::unique_ptr<INetSocket>()> socketFactory;
	std::vector<std::unique_ptr<view::IImageReader>> imageReaders;
	std::vector<std::unique_ptr<view::IModelReader>> modelReaders;
};

class Application
{
public:
	Application(Context&& context);
	~Application();

	void Run(Module&& module);
	void Run(const Path& modulePath);

	view::View& GetView();

private:
	Context m_context;
	Module m_module;
	ThreadPool m_threadPool;
	AsyncFileProvider m_asyncFileProvider;
	std::unique_ptr<model::Model> m_model;
	std::unique_ptr<view::View> m_view;
	std::unique_ptr<controller::Controller> m_controller;
	model::BoundingBoxManager m_boundingBoxManager;
	bool m_mainLoopStarted = false;
};
}