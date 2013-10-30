#include "LUAScriptHandler.h"
#include "view\GameView.h"
#include "controller\CommandHandler.h"
#include "SelectionTools.h"
#include "view\Input.h"

int CreateTable(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
        return luaL_error(L, "3 argument expected (width, height, texture)");
	double width = CLUAScript::GetArgument<double>(1);
	double height = CLUAScript::GetArgument<double>(2);
	std::string texture = CLUAScript::GetArgument<char*>(3);
	CGameView::GetIntanse().lock()->CreateTable(width, height, texture);
}

int CreateSkybox(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "2 argument expected (size, texture folder)");
	double size = CLUAScript::GetArgument<double>(1);
	std::string texture = CLUAScript::GetArgument<char*>(2);
	CGameView::GetIntanse().lock()->CreateSkybox(size, texture);
}

int CameraSetLimits(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 4)
        return luaL_error(L, "4 argument expected (max trans x, max trans y, max scale, min scale)");
	double maxTransX = CLUAScript::GetArgument<double>(1);
	double maxTransY = CLUAScript::GetArgument<double>(2);
	double maxScale = CLUAScript::GetArgument<double>(3);
	double minScale = CLUAScript::GetArgument<double>(4);
	CGameView::GetIntanse().lock()->CameraSetLimits(maxTransX, maxTransY, maxScale, minScale);
}

int CreateObject(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 4)
        return luaL_error(L, "4 argument expected (model, x, y, rotation)");
	std::string model =  CLUAScript::GetArgument<char*>(1);
	double x = CLUAScript::GetArgument<double>(2);
	double y = CLUAScript::GetArgument<double>(3);
	double rotation = CLUAScript::GetArgument<double>(4);
	CCommandHandler::GetInstance().lock()->AddNewCreateObject(model, x, y, rotation);
	return 0;
}

int DeleteSelectedObject(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
        return luaL_error(L, "no arguments expected");
	CCommandHandler::GetInstance().lock()->AddNewDeleteObject();
	return 0;
}

int RollDices(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
        return luaL_error(L, "3 argument expected (count, sides, group)");
	double count = CLUAScript::GetArgument<double>(1);
	double sides = CLUAScript::GetArgument<double>(2);
	bool group = CLUAScript::GetArgument<bool>(3);
	RollDice(count, sides, group);
}

int Ruler(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
        return luaL_error(L, "no arguments expected");
	CInput::EnableRuler();
	return 0;
}

int Undo(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
        return luaL_error(L, "no arguments expected");
	CCommandHandler::GetInstance().lock()->Undo();
	return 0;
}

int Redo(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
        return luaL_error(L, "no arguments expected");
	CCommandHandler::GetInstance().lock()->Redo();
	return 0;
}

void RegisterFunctions(CLUAScript & lua)
{
	lua.RegisterConstant(&CreateTable, "CreateTable");
	lua.RegisterConstant(&CreateSkybox, "CreateSkybox");
	lua.RegisterConstant(&CameraSetLimits, "CameraSetLimits");
	lua.RegisterConstant(&CreateObject, "CreateObject");
	lua.RegisterConstant(&DeleteSelectedObject, "DeleteObject");
	lua.RegisterConstant(&RollDices, "RollDice");
	lua.RegisterConstant(&Ruler, "Ruler");
	lua.RegisterConstant(&Undo, "Undo");
	lua.RegisterConstant(&Redo, "Redo");
}