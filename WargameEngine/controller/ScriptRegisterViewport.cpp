#include "../IScriptHandler.h"
#include "../view/View.h"
#include "ScriptRegisterFunctions.h"
#include "ScriptViewportProtocol.h"

namespace wargameEngine
{
namespace controller
{
void RegisterViewport(IScriptHandler& handler, view::View& view)
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
		return FunctionArgument(&view.AddViewport(std::make_unique<view::Viewport>(x, y, width, height, fieldOfView, view.GetViewHelper(), view.GetInput(), true, resize)), CLASS_VIEWPORT);
	});
	handler.RegisterMethod(CLASS_VIEWPORT, NEW_OFFSCREEN_VIEWPORT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("3 argument expected (width, height, textureIndex)");
		int width = args.GetInt(1);
		int height = args.GetInt(2);
		float fieldOfView = args.GetFloat(3);
		int textureSlot = args.GetInt(4);
		auto viewport = std::make_unique<view::Viewport>(0, 0, width, height, fieldOfView, view.GetViewHelper(), view.GetInput(), false, false);
		viewport->AttachNewTexture(view::IRenderer::CachedTextureType::RGBA, textureSlot);
		return FunctionArgument(&view.AddViewport(std::move(viewport)), CLASS_VIEWPORT);
	});

	handler.RegisterMethod(CLASS_VIEWPORT, CREATE_SHADOW_MAP_VIEWPORT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments (shadowMap size, shadow max Angle, x, y, z) expected");
		int size = args.GetInt(1);
		float angle = args.GetFloat(2);
		float x = args.GetFloat(3);
		float y = args.GetFloat(4);
		float z = args.GetFloat(5);
		return FunctionArgument(&view.CreateShadowMapViewport(size, angle, { x, y, z }), CLASS_VIEWPORT);
	});

	handler.RegisterMethod(CLASS_VIEWPORT, VIEWPORT_COUNT, [&](void*, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		return static_cast<long long>(view.GetViewportCount());
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
		view.RemoveViewport(static_cast<view::IViewport*>(instance));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, SET_SHADOW_MAP_VIEWPORT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(viewport)");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		auto shadowViewport = static_cast<view::Viewport*>(args.GetClassInstance(1));
		viewport->SetShadowViewport(shadowViewport);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, DISABLE_SHADOW_MAP, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		view.DisableShadowMap(*static_cast<view::Viewport*>(instance));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, ENABLE_FRUSTUM_CULLING, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (enable)");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		viewport->EnableFrustumCulling(args.GetBool(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, CAMERA_STRATEGY, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (max trans x, max trans y, max scale, min scale)");
		float maxTransX = args.GetFloat(1);
		float maxTransY = args.GetFloat(2);
		float maxScale = args.GetFloat(3);
		float minScale = args.GetFloat(4);
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		auto& camera = viewport->GetCamera();
		camera.SetLimits(maxTransX, maxTransY, 100.0f, maxScale, minScale);
		camera.SetCameraMode(view::Camera::Mode::THIRD_PERSON);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, CAMERA_FIRST_PERSON, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		viewport->GetCamera().SetCameraMode(view::Camera::Mode::FIRST_PERSON);
		viewport->GetCamera().Set(CVector3f(), CVector3f(0.0f, 1.0f, 0.0f));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, SET_CAMERA_POSITION, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 argument expected (x, y, z)");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		auto& camera = viewport->GetCamera();
		CVector3f pos(args.GetFloat(1), args.GetFloat(2), args.GetFloat(3));
		camera.SetPosition(pos);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, SET_CAMERA_TARGET, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 argument expected (x, y, z)");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		auto& camera = viewport->GetCamera();
		CVector3f target(args.GetFloat(1), args.GetFloat(2), args.GetFloat(3));
		camera.SetTarget(target);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, SET_CAMERA_UP_VECTOR, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 argument expected (x, y, z)");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		auto& camera = viewport->GetCamera();
		CVector3f up(args.GetFloat(1), args.GetFloat(2), args.GetFloat(3));
		camera.SetUpVector(up);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, CAMERA_ATTACH_TO_OBJECT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (object, offsetX, offsetY, offsetZ)");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		auto& camera = viewport->GetCamera();
		auto object = reinterpret_cast<model::IObject*>(args.GetClassInstance(1));
		CVector3f offset(args.GetFloat(2), args.GetFloat(3), args.GetFloat(4));
		camera.AttachToObject(object, offset);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, CAMERA_RESET_INPUT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		viewport->GetCamera().ResetInput();
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, CAMERA_ATTACH_KEYBOARD_MOUSE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport(0);
		viewport->GetCamera().AttachToKeyboardMouse();
		return nullptr;
	});

	handler.RegisterMethod(CLASS_VIEWPORT, GET_CAMERA, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport();
		return FunctionArgument(&viewport->GetCamera(), CLASS_CAMERA);
	});

	handler.RegisterMethod(CLASS_VIEWPORT, ENABLE_TOUCH_MODE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected ()");
		auto viewport = instance ? static_cast<view::Viewport*>(instance) : &view.GetViewport();
		viewport->GetCamera().AttachToTouchScreen();
		return nullptr;
	});
}
}
}