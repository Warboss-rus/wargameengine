#include "..\model\GameModel.h"
#include "..\controller\CommandHandler.h"
#include "LUAScriptHandler.h"
#include "..\3dObject.h"

int NewObject(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 5)
        return luaL_error(L, "4 argument expected (model, x, y, rotation)");
	std::string model =  CLUAScript::GetArgument<char*>(2);
	double x = CLUAScript::GetArgument<double>(3);
	double y = CLUAScript::GetArgument<double>(4);
	double rotation = CLUAScript::GetArgument<double>(5);
	IObject* object = new C3DObject(model, x, y, rotation);
	CCommandHandler::GetInstance().lock()->AddNewCreateObject(std::shared_ptr<IObject>(object));
	return CLUAScript::NewInstanceClass(object, "Object");
}

int GetSelectedObject(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no argument expected");
	IObject * object = CGameModel::GetIntanse().lock()->GetSelectedObject().get();
	return CLUAScript::NewInstanceClass(object, "Object");
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

int MoveObjectTo(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 4)
        return luaL_error(L, "3 argument expected(x, y, rotation)");
	IObject * object = (IObject *)CLUAScript::GetClassInstance("Object");
	object->MoveTo(CLUAScript::GetArgument<double>(2), CLUAScript::GetArgument<double>(3), CLUAScript::GetArgument<double>(4));
	return 0;
}

static const luaL_Reg ObjectFuncs[] = {
	{ "New", NewObject },
	{ "GetSelected", GetSelectedObject },
	{ "Delete", DeleteObject },
	{ "Null", ObjectNull },
	{ "GetModel", GetObjectModel },
	{ "GetX", GetObjectX },
	{ "GetY", GetObjectY },
	{ "GetZ", GetObjectZ },
	{ "GetRotation", GetObjectRotation },
	{ "MoveTo", MoveObjectTo },
	{ NULL, NULL }
};

void RegisterObject(CLUAScript & lua)
{
	lua.RegisterClass(ObjectFuncs, "Object");
}