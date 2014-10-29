#include "../model/GameModel.h"
#include "../controller/CommandHandler.h"
#include "LUAScriptHandler.h"
#include "../model/Object.h"
#include "../model/ObjectGroup.h"
#include "../view/GameView.h"

int NewObject(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 5)
        return luaL_error(L, "4 argument expected (model, x, y, rotation)");
	std::string model =  CLUAScript::GetArgument<char*>(2);
	double x = CLUAScript::GetArgument<double>(3);
	double y = CLUAScript::GetArgument<double>(4);
	double rotation = CLUAScript::GetArgument<double>(5);
	IObject* object = new CObject(model, x, y, rotation);
	CCommandHandler::GetInstance().lock()->AddNewCreateObject(std::shared_ptr<IObject>(object));
	return CLUAScript::NewInstanceClass(object, "Object");
}

int NewDecal(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 7)
        return luaL_error(L, "6 argument expected (decal, x, y, rotation, width, height)");
	std::string model =  CLUAScript::GetArgument<char*>(2);
	double x = CLUAScript::GetArgument<double>(3);
	double y = CLUAScript::GetArgument<double>(4);
	double rotation = CLUAScript::GetArgument<double>(5);
	double width = CLUAScript::GetArgument<double>(6);
	double height = CLUAScript::GetArgument<double>(7);
	char temp[10];
	sprintf(temp, "%f", height);
	model = std::string(temp) + ' ' + model;
	sprintf(temp, "%f", width);
	model = std::string(temp) + ' ' + model;
	IObject* object = new CObject(model, x, y, rotation, false);
	CCommandHandler::GetInstance().lock()->AddNewCreateObject(std::shared_ptr<IObject>(object));
	return CLUAScript::NewInstanceClass(object, "Object");
}

int GetSelectedObject(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = CGameModel::GetInstance().lock()->GetSelectedObject().get();
	return CLUAScript::NewInstanceClass(object, "Object");
}

int GetCount(lua_State* L)
{
	CLUAScript::SetArgument((int)CGameModel::GetInstance().lock()->GetObjectCount());
	return 1;
}

int GetAt(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "1 argument expected(index)");
	size_t index = CLUAScript::GetArgument<int>(2);
	if (index > CGameModel::GetInstance().lock()->GetObjectCount()) CLUAScript::NewInstanceClass(NULL, "Object");
	return CLUAScript::NewInstanceClass(CGameModel::GetInstance().lock()->Get3DObject(index - 1).get(), "Object");
}

int DeleteObject(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	CCommandHandler::GetInstance().lock()->AddNewDeleteObject(std::shared_ptr<IObject>(object));
	object = nullptr;
	return 0;
}

int ObjectNull(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	CLUAScript::SetArgument(object == NULL);
	return 1;
}

int GetObjectModel(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	CLUAScript::SetArgument(object->GetPathToModel().c_str());
	return 1;
}

int GetObjectX(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	CLUAScript::SetArgument(object->GetX());
	return 1;
}

int GetObjectY(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	CLUAScript::SetArgument(object->GetY());
	return 1;
}

int GetObjectZ(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	CLUAScript::SetArgument(object->GetZ());
	return 1;
}

int GetObjectRotation(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	CLUAScript::SetArgument(object->GetRotation());
	return 1;
}

int MoveObject(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 4)
        return luaL_error(L, "3 arguments expected(x, y, z)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	object->Move(CLUAScript::GetArgument<double>(2), CLUAScript::GetArgument<double>(3), CLUAScript::GetArgument<double>(4));
	return 0;
}

int RotateObject(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected(rotation)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	object->Rotate(CLUAScript::GetArgument<double>(2));
	return 0;
}

int ShowMesh(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected(meshname)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	object->ShowMesh(CLUAScript::GetArgument<char*>(2));
	return 0;
}

int HideMesh(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected(meshname)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	object->HideMesh(CLUAScript::GetArgument<char*>(2));
	return 0;
}

int GetProperty(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "1 argument expected (key)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	char* key = CLUAScript::GetArgument<char*>(2);
	CLUAScript::SetArgument(object->GetProperty(key).c_str());
	return 1;
}

int SetProperty(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 3)
		return luaL_error(L, "2 arguments expected (key, value)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	char* key = CLUAScript::GetArgument<char*>(2);
	char* value = CLUAScript::GetArgument<char*>(3);
	std::shared_ptr<IObject> obj = CGameModel::GetInstance().lock()->Get3DObject(object);
	CCommandHandler::GetInstance().lock()->AddNewChangeProperty(obj, key, value);
	return 0;
}

int SetSelectable(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected(isSelectable)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	object->SetSelectable(CLUAScript::GetArgument<bool>(2));
	return 0;
}

int SetMoveLimit(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() < 2)
        return luaL_error(L, "at least 1 argument expected(moveLimiterType)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	std::string limiterType = CLUAScript::GetArgument<const char*>(2);
	if(limiterType == "free")
	{
		if (CLUAScript::GetArgumentCount() != 3)//I have no idea why arguments are +1
			return luaL_error(L, "1 argument expected(moveLimiterType)");
		object->SetMovementLimiter(NULL);
	}
	if(limiterType == "static")
	{
		if (CLUAScript::GetArgumentCount() != 3)
			return luaL_error(L, "1 argument expected(moveLimiterType)");
		object->SetMovementLimiter(new CMoveLimiterStatic(object->GetX(), object->GetY(), object->GetZ(), object->GetRotation()));
	}
	if(limiterType == "circle")
	{
		if (CLUAScript::GetArgumentCount() != 6)
			return luaL_error(L, "4 argument expected(moveLimiterType, centerX, centerY, radius)");
		double centerX = CLUAScript::GetArgument<double>(3);
		double centerY = CLUAScript::GetArgument<double>(4);
		double radius = CLUAScript::GetArgument<double>(5);
		object->SetMovementLimiter(new CMoveLimiterCircle(centerX, centerY, radius));
	}
	if(limiterType == "rectangle")
	{
		int n =CLUAScript::GetArgumentCount();
		if (CLUAScript::GetArgumentCount() != 7)
			return luaL_error(L, "5 argument expected(moveLimiterType, x1, y1, x2, y2)");
		double x1 = CLUAScript::GetArgument<double>(3);
		double y1 = CLUAScript::GetArgument<double>(4);
		double x2 = CLUAScript::GetArgument<double>(5);
		double y2 = CLUAScript::GetArgument<double>(6);
		object->SetMovementLimiter(new CMoveLimiterRectangle(x1, y1, x2, y2));
	}
	return 0;
}

int SelectNull(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	std::shared_ptr<IObject> object = NULL;
	CGameModel::GetInstance().lock()->SelectObject(object);
	return 0;
}

int ObjectEquals(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (secondObject)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	IObject * obj2= (IObject*)CLUAScript::GetArgument<void*>(2);
	CLUAScript::SetArgument(object == obj2);
	return 1;
}

int IsGroup(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	CLUAScript::SetArgument(CGameModel::IsGroup(object));
	return 1;
}

int GetGroupChildrenCount(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	if(!object) CLUAScript::SetArgument(0);//NULL contains no objects
	if(!CGameModel::IsGroup(object)) CLUAScript::SetArgument(1); //single object
	CObjectGroup * group = (CObjectGroup *)object;
	CLUAScript::SetArgument((int)group->GetCount());
	return 1;
}

int GetGroupChildrenAt(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected(index)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	size_t index = CLUAScript::GetArgument<int>(2);
	if(!object && !CGameModel::IsGroup(object)) CLUAScript::NewInstanceClass(NULL, "Object");
	CObjectGroup * group = (CObjectGroup *)object;
	if(index > group->GetCount()) CLUAScript::NewInstanceClass(NULL, "Object");
	CLUAScript::NewInstanceClass(group->GetChild(index - 1).get(), "Object");
	return 1;
}

int PlayAnimation(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "1 argument expected (animation)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	char* anim = CLUAScript::GetArgument<char*>(2);
	if (!object)
	{
		luaL_error(L, "needs to be called on valid object");
	}
	object->PlayAnimation(anim);
	return 0;
}

int GetAnimations(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "no arguments expected");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	std::vector<std::string> anims = CGameView::GetInstance().lock()->GetModelManager()->GetAnimations(object->GetPathToModel());
	CLUAScript::SetArray(anims);
	return 1;
}

static const luaL_Reg ObjectFuncs[] = {
	{ "New", NewObject },
	{ "NewDecal", NewDecal },
	{ "GetSelected", GetSelectedObject },
	{ "GetCount", GetCount },
	{ "GetAt", GetAt },
	{ "Delete", DeleteObject },
	{ "Null", ObjectNull },
	{ "GetModel", GetObjectModel },
	{ "GetX", GetObjectX },
	{ "GetY", GetObjectY },
	{ "GetZ", GetObjectZ },
	{ "GetRotation", GetObjectRotation },
	{ "Move", MoveObject },
	{ "Rotate", RotateObject },
	{ "ShowMesh", ShowMesh },
	{ "HideMesh", HideMesh },
	{ "GetProperty", GetProperty },
	{ "SetProperty", SetProperty },
	{ "SetSelectable", SetSelectable },
	{ "SetMoveLimit", SetMoveLimit },
	{ "SelectNull", SelectNull },
	{ "Equals", ObjectEquals },
	{ "IsGroup", IsGroup },
	{ "GetGroupChildrenCount", GetGroupChildrenCount },
	{ "GetGroupChildrenAt", GetGroupChildrenAt },
	{ "PlayAnimation", PlayAnimation },
	{ "GetAnimations", GetAnimations },
	{ NULL, NULL }
};

void RegisterObject(CLUAScript & lua)
{
	lua.RegisterClass(ObjectFuncs, "Object");
}