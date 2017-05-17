#include "../IScriptHandler.h"
#include "../controller/Controller.h"
#include "../model/Model.h"
#include "../model/Object.h"
#include "../model/ObjectGroup.h"
#include "../view/ModelManager.h"
#include "MovementLimiter.h"
#include "ScriptObjectProtocol.h"
#include "ScriptRegisterFunctions.h"
#include <algorithm>

namespace wargameEngine
{
namespace controller
{

void RegisterObject(IScriptHandler& handler, Controller& controller, model::Model& model, view::ModelManager& modelManager)
{
	handler.RegisterMethod(CLASS_OBJECT, NEW_OBJECT, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (model, x, y, rotation)");
		Path model = args.GetPath(1);
		float x = args.GetFloat(2);
		float y = args.GetFloat(3);
		float rotation = args.GetFloat(4);
		std::shared_ptr<model::IObject> object = controller.CreateObject(model, x, y, rotation);
		return FunctionArgument(object.get(), L"Object");
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_SELECTED_OBJECT, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		model::IObject* object = model.GetSelectedObject().get();
		return FunctionArgument(object, L"Object");
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_COUNT, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		return (int)model.GetObjectCount();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_AT, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(index)");
		size_t index = args.GetSizeT(1);
		if (index > model.GetObjectCount())
			return FunctionArgument();
		return FunctionArgument(model.Get3DObject(index - 1).get(), L"Object");
	});

	handler.RegisterMethod(CLASS_OBJECT, DELETE_OBJECT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		if (!instance)
			throw std::runtime_error("should be called with a valid instance");
		std::shared_ptr<model::IObject> shared_object = model.Get3DObject((model::IObject*)instance);
		controller.DeleteObject(shared_object);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_MODEL, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetPathToModel();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_X, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetX();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_Y, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetY();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_Z, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetZ();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_ROTATION, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetRotation();
	});

	handler.RegisterMethod(CLASS_OBJECT, MOVE_OBJECT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected(x, y, z)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->Move(args.GetFloat(1), args.GetFloat(2), args.GetFloat(3));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, ROTATE_OBJECT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(rotation)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->Rotate(args.GetFloat(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, SHOW_MESH, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(meshname)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->ShowMesh(args.GetStr(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, HIDE_MESH, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(meshname)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->HideMesh(args.GetStr(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_PROPERTY, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (key)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::wstring key = args.GetWStr(1);
		return object->GetProperty(key);
	});

	handler.RegisterMethod(CLASS_OBJECT, SET_PROPERTY, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 arguments expected (key, value)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::wstring key = args.GetWStr(1);
		std::wstring value = args.GetWStr(2);
		std::shared_ptr<model::IObject> obj = model.Get3DObject(object);
		controller.SetObjectProperty(obj, key, value);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, SET_SELECTABLE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(isSelectable)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->SetSelectable(args.GetBool(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, SET_MOVE_LIMIT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() < 1)
			throw std::runtime_error("at least 1 argument expected(moveLimiterType)");
		auto object = model.Get3DObject(reinterpret_cast<model::IObject*>(instance));
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::string limiterType = args.GetStr(1);
		if (limiterType == "free")
		{
			if (args.GetCount() != 1) //I have no idea why arguments are +1
				throw std::runtime_error("1 argument expected(moveLimiterType)");
			controller.SetMovementLimiter(object, NULL);
		}
		if (limiterType == "static")
		{
			if (args.GetCount() != 1)
				throw std::runtime_error("1 argument expected(moveLimiterType)");
			controller.SetMovementLimiter(object, std::make_unique<MoveLimiterStatic>());
		}
		if (limiterType == "circle")
		{
			if (args.GetCount() != 4)
				throw std::runtime_error("4 argument expected(moveLimiterType, centerX, centerY, radius)");
			float centerX = args.GetFloat(2);
			float centerY = args.GetFloat(3);
			float radius = args.GetFloat(4);
			controller.SetMovementLimiter(object, std::make_unique<MoveLimiterCircle>(centerX, centerY, radius));
		}
		if (limiterType == "rectangle")
		{
			if (args.GetCount() != 5)
				throw std::runtime_error("5 argument expected(moveLimiterType, x1, y1, x2, y2)");
			float x1 = args.GetFloat(2);
			float y1 = args.GetFloat(3);
			float x2 = args.GetFloat(4);
			float y2 = args.GetFloat(5);
			controller.SetMovementLimiter(object, std::make_unique<MoveLimiterRectangle>(x1, y1, x2, y2));
		}
		if (limiterType == "tiles")
		{
			if (args.GetCount() != 1)
				throw std::runtime_error("1 argument expected(moveLimiterType)");
			controller.SetMovementLimiter(object, std::make_unique<MoveLimiterTiles>());
		}
		if (limiterType == "custom")
		{
			if (args.GetCount() != 3)
				throw std::runtime_error("3 arguments expected(moveLimiterType, functionName)");
			std::wstring functionName = args.GetWStr(1);
			auto function = [&, functionName](CVector3f& position, CVector3f& rotation, const CVector3f& oldPosition, const CVector3f& oldRotation) {
				auto vector3FConvert = [](CVector3f const& vec) {
					return std::vector<FunctionArgument>{ vec.x, vec.y, vec.z };
				};
				handler.CallFunction(functionName, { vector3FConvert(position), vector3FConvert(rotation), vector3FConvert(oldPosition), vector3FConvert(oldRotation) });
				//TODO: get updated values from the function return
				return true;
			};
			controller.SetMovementLimiter(object, std::make_unique<CustomMoveLimiter>(function));
		}
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, SELECT_OBJECT, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (object to select)");
		std::shared_ptr<model::IObject> object = model.Get3DObject((model::IObject*)args.GetClassInstance(1));
		model.SelectObject(object);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, OBJECT_EQUALS, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (secondObject)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		model::IObject* obj2 = (model::IObject*)args.GetClassInstance(1);
		return object == obj2;
	});

	handler.RegisterMethod(CLASS_OBJECT, OBJECT_IS_GROUP, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->IsGroup();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_GROUP_CHILDREN_COUNT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		if (object)
		{
			if (!object->IsGroup())
				return 1; //single object
			model::ObjectGroup* group = (model::ObjectGroup*)object;
			return (int)group->GetCount();
		}
		return 0; //NULL contains no objects
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_GROUP_CHILDREN_AT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(index)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		size_t index = args.GetSizeT(1);
		if (object && object->IsGroup())
		{
			model::ObjectGroup* group = (model::ObjectGroup*)object;
			if (index > group->GetCount())
				FunctionArgument(NULL, L"Object");
			FunctionArgument(group->GetChild(index - 1).get(), L"Object");
		}
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, PLAY_ANIMATION, [&](void* instance, IArguments const& args) {
		int n = args.GetCount();
		if (n < 1 || n > 3)
			throw std::runtime_error("1 to 3 argument expected (animation, loop mode, speed)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::string anim = args.GetStr(1);
		std::string sloop = "nonlooping";
		float speed = 1.0f;
		if (n == 2)
			sloop = args.GetStr(2);
		if (n == 3)
			speed = args.GetFloat(3);
		transform(sloop.begin(), sloop.end(), sloop.begin(), [](char c) { return static_cast<char>(::tolower(c)); });
		model::AnimationLoop loop = model::AnimationLoop::NonLooping;
		if (sloop == "looping")
			loop = model::AnimationLoop::Looping;
		if (sloop == "holdend")
			loop = model::AnimationLoop::HoldEnd;
		if (!object)
		{
			throw std::runtime_error("needs to be called on valid object");
		}
		auto objectPtr = model.Get3DObject(object);
		controller.PlayObjectAnimation(objectPtr, anim, loop, speed);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_ANIMATIONS, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::vector<std::string> anims = modelManager.GetAnimations(object->GetPathToModel());
		return TransformVector(anims);
	});

	handler.RegisterMethod(CLASS_OBJECT, ADDITIONAL_MODEL, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(model)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->AddSecondaryModel(args.GetPath(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, REMOVE_ADDITIONAL_MODEL, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(model)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->RemoveSecondaryModel(args.GetPath(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, GO_TO, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected(x, y, speed, animation, animationSpeed)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		float x = args.GetFloat(1);
		float y = args.GetFloat(2);
		float speed = args.GetFloat(3);
		std::string anim = args.GetStr(4);
		float animSpeed = args.GetFloat(5);
		auto objectPtr = model.Get3DObject(object);
		controller.ObjectGoTo(objectPtr, x, y, speed, anim, animSpeed);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, APPLY_TEAMCOLOR, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected(mask suffix, r, g, b)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::wstring suffix = args.GetWStr(1);
		unsigned char r = static_cast<unsigned char>(args.GetInt(2));
		unsigned char g = static_cast<unsigned char>(args.GetInt(3));
		unsigned char b = static_cast<unsigned char>(args.GetInt(4));
		object->ApplyTeamColor(suffix, r, g, b);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, REPLACE_TEXTURE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected(old texture, new texture)");
		model::IObject* object = reinterpret_cast<model::IObject*>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		Path oldt = args.GetPath(1);
		Path newt = args.GetPath(2);
		object->ReplaceTexture(oldt, newt);
		return nullptr;
	});

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_X, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(x)");
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		float x = args.GetFloat(1);
		object->SetCoords(x, object->GetY(), object->GetZ()); }, [](void* instance) {
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->GetX(); });

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_Y, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(y)");
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		float y = args.GetFloat(1);
		object->SetCoords(object->GetX(), y, object->GetZ()); }, [](void* instance) {
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->GetY(); });

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_Z, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(z)");
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		float z = args.GetFloat(1);
		object->SetCoords(object->GetX(), object->GetY(), z); }, [](void* instance) {
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->GetZ(); });

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_ROTATION, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(rotation)");
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		float rotation = args.GetFloat(1);
		object->Rotate(object->GetRotation() - rotation); }, [](void* instance) {
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->GetRotation(); });

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_SELECTABLE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(selectable)");
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		object->SetSelectable(args.GetBool(1)); }, [](void* instance) {
		model::IObject* object = reinterpret_cast<model::IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->IsSelectable(); });

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_COUNT, [&](void* /*instance*/) {
		return (int)model.GetObjectCount();
	});
}
}
}