#include "ScriptRegisterFunctions.h"
#include "ScriptViewportProtocol.h"
#include "../view/Viewport.h"
#include "../view/OffscreenViewport.h"
#include "../view/CameraStrategy.h"
#include "../view/CameraFirstPerson.h"
#include "IScriptHandler.h"
#include "../view/GameView.h"

void RegisterViewport(IScriptHandler & handler, CGameView & view)
{
	handler.RegisterMethod(CLASS_VIEWPORT, NEW_VIEWPORT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 5 && args.GetCount() != 6)
			throw std::runtime_error("5 to 6 argument expected (x, y, width, height, fieldOfView, [resize])");
		int x = args.GetInt(0);
		int y = args.GetInt(1);
		int width = args.GetInt(2);
		int height = args.GetInt(3);
		float fieldOfView = args.GetFloat(4);
		bool resize = (args.GetCount() > 5) ? args.GetBool(5) : false;
		return FunctionArgument(&view.AddViewport(std::make_unique<CViewport>(x, y, width, height, fieldOfView, view.GetViewHelper(), resize)), CLASS_VIEWPORT);
	});
	handler.RegisterMethod(CLASS_VIEWPORT, NEW_OFFSCREEN_VIEWPORT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("3 argument expected (width, height, textureIndex)");
		int width = args.GetInt(1);
		int height = args.GetInt(2);
		float fieldOfView = args.GetFloat(3);
		int textureSlot = args.GetInt(4);
		return FunctionArgument(&view.AddViewport(std::make_unique<COffscreenViewport>(CachedTextureType::RGBA, width, height, fieldOfView, view.GetViewHelper(), textureSlot)), CLASS_VIEWPORT);
	});

	handler.RegisterMethod(CLASS_VIEWPORT, VIEWPORT_COUNT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		return static_cast<int>(view.GetViewportCount());
	});

	handler.RegisterMethod(CLASS_VIEWPORT, GET_VIEWPORT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		int index = args.GetInt(1);
		return FunctionArgument(&view.GetViewport(index), CLASS_VIEWPORT);
	});

	handler.RegisterMethod(CLASS_VIEWPORT, REMOVE_VIEWPORT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.RemoveViewport(static_cast<IViewport*>(instance));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, CAMERA_STRATEGY, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (max trans x, max trans y, max scale, min scale)");
		double maxTransX = args.GetDbl(1);
		double maxTransY = args.GetDbl(2);
		double maxScale = args.GetDbl(3);
		double minScale = args.GetDbl(4);
		auto viewport = instance ? static_cast<IViewport*>(instance) : &view.GetViewport(0);
		viewport->SetCamera(std::make_unique<CCameraStrategy>(maxTransX, maxTransY, maxScale, minScale));
		viewport->GetCamera().SetInput(view.GetInput());
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, CAMERA_FIRST_PERSON, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		auto viewport = instance ? static_cast<IViewport*>(instance) : &view.GetViewport(0);
		viewport->SetCamera(std::make_unique<CCameraFirstPerson>());
		viewport->GetCamera().SetInput(view.GetInput());
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, GET_CAMERA, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		auto viewport = static_cast<IViewport*>(instance);
		return FunctionArgument(&viewport->GetCamera(), CLASS_CAMERA);
	});
}