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

	handler.RegisterMethod(CLASS_VIEWPORT, CREATE_SHADOW_MAP_VIEWPORT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments (shadowMap size, shadow max Angle, x, y, z) expected");
		int size = args.GetInt(1);
		float angle = args.GetFloat(2);
		float x = args.GetFloat(3);
		float y = args.GetFloat(4);
		float z = args.GetFloat(5);
		return FunctionArgument(&view.CreateShadowMapViewport(size, angle, {x, y, z}), CLASS_VIEWPORT);
	});

	handler.RegisterMethod(CLASS_VIEWPORT, VIEWPORT_COUNT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		return static_cast<int>(view.GetViewportCount());
	});

	handler.RegisterMethod(CLASS_VIEWPORT, GET_VIEWPORT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		size_t index = args.GetSizeT(1);
		return FunctionArgument(&view.GetViewport(index), CLASS_VIEWPORT);
	});

	handler.RegisterMethod(CLASS_VIEWPORT, REMOVE_VIEWPORT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.RemoveViewport(static_cast<IViewport*>(instance));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, SET_SHADOW_MAP_VIEWPORT, [&](void* instance, IArguments const& args){
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(viewport)");
		auto viewport = instance ? static_cast<IViewport*>(instance) : &view.GetViewport(0);
		auto shadowViewport = static_cast<IViewport*>(args.GetClassInstance(1));
		viewport->SetShadowViewport(shadowViewport);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, DISABLE_SHADOW_MAP, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.DisableShadowMap(*static_cast<IViewport*>(instance));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, CAMERA_STRATEGY, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (max trans x, max trans y, max scale, min scale)");
		float maxTransX = args.GetFloat(1);
		float maxTransY = args.GetFloat(2);
		float maxScale = args.GetFloat(3);
		float minScale = args.GetFloat(4);
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
		auto viewport = instance ? static_cast<IViewport*>(instance) : &view.GetViewport();
		return FunctionArgument(&viewport->GetCamera(), CLASS_CAMERA);
	});

	handler.RegisterMethod(CLASS_VIEWPORT, ENABLE_TOUCH_MODE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		auto viewport = instance ? static_cast<IViewport*>(instance) : &view.GetViewport();
		viewport->GetCamera().EnableTouchMode();
		return nullptr;
	});
}