#include "ScriptRegisterFunctions.h"
#include "ScriptObjectProtocol.h"
#include "IScriptHandler.h"
#include "../model/GameModel.h"
#include "../controller/GameController.h"
#include "../model/Object.h"
#include "../model/ObjectGroup.h"
#include "../view/GameView.h"
#include <algorithm>


void RegisterObject(IScriptHandler & handler, CGameController & controller, CGameModel & model)
{
	handler.RegisterMethod(CLASS_OBJECT, NEW_OBJECT, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected (model, x, y, rotation)");
		std::string model =  args.GetStr(1);
		double x = args.GetDbl(2);
		double y = args.GetDbl(3);
		double rotation = args.GetDbl(4);
		std::shared_ptr<IObject> object = controller.CreateObject(model, x, y, rotation);
		return FunctionArgument(object.get(), "Object");
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_SELECTED_OBJECT, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		IObject * object = model.GetSelectedObject().get();
		return FunctionArgument(object, "Object");
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_COUNT, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		return (long)model.GetObjectCount();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_AT, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(index)");
		size_t index = args.GetLong(1);
		if (index > model.GetObjectCount()) return FunctionArgument();
		return FunctionArgument(model.Get3DObject(index - 1).get(), "Object");
	});

	handler.RegisterMethod(CLASS_OBJECT, DELETE_OBJECT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		if (!instance)
			throw std::runtime_error("should be called with a valid instance");
		std::shared_ptr<IObject> shared_object = model.Get3DObject((IObject *)instance);
		controller.DeleteObject(shared_object);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_MODEL, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetPathToModel();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_X, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetX();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_Y, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetY();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_Z, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetZ();
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_OBJECT_ROTATION, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return object->GetRotation();
	});

	handler.RegisterMethod(CLASS_OBJECT, MOVE_OBJECT, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 3)
			throw std::runtime_error("3 arguments expected(x, y, z)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->Move(args.GetDbl(1), args.GetDbl(2), args.GetDbl(3));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, ROTATE_OBJECT, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(rotation)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->Rotate(args.GetDbl(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, SHOW_MESH, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(meshname)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->ShowMesh(args.GetStr(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, HIDE_MESH, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(meshname)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->HideMesh(args.GetStr(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_PROPERTY, [&](void* instance, IArguments const& args)
	{
		if(args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (key)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::string key = args.GetStr(1);
		return object->GetProperty(key);
	});

	handler.RegisterMethod(CLASS_OBJECT, SET_PROPERTY, [&](void* instance, IArguments const& args)
	{
		if(args.GetCount() != 2)
			throw std::runtime_error("2 arguments expected (key, value)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::string key = args.GetStr(1);
		std::string value = args.GetStr(2);
		std::shared_ptr<IObject> obj = model.Get3DObject(object);
		controller.SetObjectProperty(obj, key, value);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, SET_SELECTABLE, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(isSelectable)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->SetSelectable(args.GetBool(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, SET_MOVE_LIMIT, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() < 1)
			throw std::runtime_error("at least 1 argument expected(moveLimiterType)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::string limiterType = args.GetStr(1);
		if(limiterType == "free")
		{
			if (args.GetCount() != 1)//I have no idea why arguments are +1
				throw std::runtime_error("1 argument expected(moveLimiterType)");
			object->SetMovementLimiter(NULL);
		}
		if(limiterType == "static")
		{
			if (args.GetCount() != 1)
				throw std::runtime_error("1 argument expected(moveLimiterType)");
			object->SetMovementLimiter(new CMoveLimiterStatic(object->GetX(), object->GetY(), object->GetZ(), object->GetRotation()));
		}
		if(limiterType == "circle")
		{
			if (args.GetCount() != 4)
				throw std::runtime_error("4 argument expected(moveLimiterType, centerX, centerY, radius)");
			double centerX = args.GetDbl(2);
			double centerY = args.GetDbl(3);
			double radius = args.GetDbl(4);
			object->SetMovementLimiter(new CMoveLimiterCircle(centerX, centerY, radius));
		}
		if(limiterType == "rectangle")
		{
			if (args.GetCount() != 5)
				throw std::runtime_error("5 argument expected(moveLimiterType, x1, y1, x2, y2)");
			double x1 = args.GetDbl(2);
			double y1 = args.GetDbl(3);
			double x2 = args.GetDbl(4);
			double y2 = args.GetDbl(5);
			object->SetMovementLimiter(new CMoveLimiterRectangle(x1, y1, x2, y2));
		}
		if (limiterType == "tiles")
		{
			if (args.GetCount() != 1)
				throw std::runtime_error("1 argument expected(moveLimiterType)");
			object->SetMovementLimiter(new CMoveLimiterTiles());
		}
		if (limiterType == "custom")
		{
			if (args.GetCount() != 3)
				throw std::runtime_error("3 arguments expected(moveLimiterType, functionName)");
			std::string functionName = args.GetStr(1);
			auto function = [&, functionName](CVector3d & position, double & rotation, const CVector3d & oldPosition, double oldRotation) {
				auto vector3DConvert = [](CVector3d const& vec) {
					return std::vector<double>{vec.x, vec.y, vec.z};
				};
				handler.CallFunction(functionName, { vector3DConvert(position), rotation, vector3DConvert(oldPosition), oldRotation });
				//TODO: get updated values from the function return
			};
			object->SetMovementLimiter(new CCustomMoveLimiter(function));
		}
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, SELECT_OBJECT, [&](void* /*instance*/, IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (object to select)");
		std::shared_ptr<IObject> object = model.Get3DObject((IObject *)args.GetClassInstance(1));
		model.SelectObject(object);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, OBJECT_EQUALS, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (secondObject)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		IObject * obj2= (IObject*)args.GetClassInstance(1);
		return object == obj2;
	});

	handler.RegisterMethod(CLASS_OBJECT, OBJECT_IS_GROUP, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		return CGameModel::IsGroup(object);
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_GROUP_CHILDREN_COUNT, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no argument expected");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		if (object)
		{
			if (!CGameModel::IsGroup(object)) return 1l; //single object
			CObjectGroup * group = (CObjectGroup *)object;
			return (long)group->GetCount();
		}
		return 0l;//NULL contains no objects
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_GROUP_CHILDREN_AT, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(index)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		size_t index = args.GetLong(1);
		if (object && CGameModel::IsGroup(object))
		{
			CObjectGroup * group = (CObjectGroup *)object;
			if (index > group->GetCount()) FunctionArgument(NULL, "Object");
			FunctionArgument(group->GetChild(index - 1).get(), "Object");
		}
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, PLAY_ANIMATION, [&](void* instance, IArguments const& args)
	{
		int n = args.GetCount();
		if (n < 1 || n > 3)
			throw std::runtime_error("1 to 3 argument expected (animation, loop mode, speed)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::string anim = args.GetStr(1);
		std::string sloop = "nonlooping";
		float speed = 1.0f;
		if (n == 2) sloop = args.GetStr(2);
		if (n == 3) speed = args.GetFloat(3);
		transform(sloop.begin(), sloop.end(), sloop.begin(), ::tolower);
		sAnimation::eLoopMode loop = sAnimation::NONLOOPING;
		if (sloop == "looping") loop = sAnimation::LOOPING;
		if (sloop == "holdend") loop = sAnimation::HOLDEND;
		if (!object)
		{
			throw std::runtime_error("needs to be called on valid object");
		}
		auto objectPtr = model.Get3DObject(object);
		controller.PlayObjectAnimation(objectPtr, anim, loop, speed);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, GET_ANIMATIONS, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::vector<std::string> anims = CGameView::GetInstance().lock()->GetModelManager().GetAnimations(object->GetPathToModel());
		return anims;
	});

	handler.RegisterMethod(CLASS_OBJECT, ADDITIONAL_MODEL, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(model)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->AddSecondaryModel(args.GetStr(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, REMOVE_ADDITIONAL_MODEL, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected(model)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		object->RemoveSecondaryModel(args.GetStr(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, GO_TO, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 5)
			throw std::runtime_error("5 argument expected(x, y, speed, animation, animationSpeed)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		double x = args.GetDbl(1);
		double y = args.GetDbl(2);
		double speed = args.GetDbl(3);
		std::string anim = args.GetStr(4);
		float animSpeed = args.GetFloat(5);
		auto objectPtr = model.Get3DObject(object);
		controller.ObjectGoTo(objectPtr, x, y, speed, anim, animSpeed);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, APPLY_TEAMCOLOR, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 4)
			throw std::runtime_error("4 argument expected(mask suffix, r, g, b)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::string suffix = args.GetStr(1);
		unsigned char r = static_cast<unsigned char>(args.GetInt(2));
		unsigned char g = static_cast<unsigned char>(args.GetInt(3));
		unsigned char b = static_cast<unsigned char>(args.GetInt(4));
		object->ApplyTeamColor(suffix, r, g, b);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_OBJECT, REPLACE_TEXTURE, [&](void* instance, IArguments const& args)
	{
		if (args.GetCount() != 2)
			throw std::runtime_error("2 argument expected(old texture, new texture)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object)
			throw std::runtime_error("should be called with a valid instance");
		std::string oldt = args.GetStr(1);
		std::string newt = args.GetStr(2);
		object->ReplaceTexture(oldt, newt);
		return nullptr;
	});

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_X, [&](void * instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(x)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		double x = args.GetDbl(1);
		object->SetCoords(x, object->GetY(), object->GetZ());
	}, [](void* instance) {
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->GetX();
	});

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_Y, [&](void * instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(y)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		double y = args.GetDbl(1);
		object->SetCoords(object->GetX(), y, object->GetZ());
	}, [](void* instance) {
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->GetY();
	});

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_Z, [&](void * instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(z)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		double z = args.GetDbl(1);
		object->SetCoords(object->GetX(), object->GetY(), z);
	}, [](void* instance) {
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->GetZ();
	});

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_ROTATION, [&](void * instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(rotation)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		double rotation = args.GetDbl(1);
		object->Rotate(object->GetRotation() - rotation);
	}, [](void* instance) {
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->GetRotation();
	});

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_SELECTABLE, [&](void * instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected(selectable)");
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		object->SetSelectable(args.GetBool(1));
	}, [](void* instance) {
		IObject* object = reinterpret_cast<IObject *>(instance);
		if (!object) throw std::runtime_error("should be called with a valid instance");
		return object->IsSelectable();
	});

	handler.RegisterProperty(CLASS_OBJECT, PROPERTY_COUNT, [&](void* /*instance*/) {
		return (long)model.GetObjectCount();
	});
}