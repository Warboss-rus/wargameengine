#include "LUAScriptHandler.h"
#include "..\view\GameView.h"
#include "..\controller\CommandHandler.h"
#include "..\view\Input.h"
#include <Windows.h>
#include "..\los.h"
#include <gl\glut.h>
#include "TimedCallback.h"

int CreateTable(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
        return luaL_error(L, "3 argument expected (width, height, texture)");
	float width = CLUAScript::GetArgument<float>(1);
	float height = CLUAScript::GetArgument<float>(2);
	std::string texture = CLUAScript::GetArgument<char*>(3);
	CGameView::GetIntanse().lock()->CreateTable(width, height, texture);
	return 0;
}

int CreateSkybox(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "2 argument expected (size, texture folder)");
	double size = CLUAScript::GetArgument<double>(1);
	std::string texture = CLUAScript::GetArgument<char*>(2);
	CGameView::GetIntanse().lock()->CreateSkybox(size, texture);
	return 0;
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
	return 0;
}

int LoS(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "2 argument expected (max trans x, max trans y, max scale, min scale)");
	IObject* shootingModel = (IObject*)CLUAScript::GetArgument<void*>(1);
	IObject* target = (IObject*)CLUAScript::GetArgument<void*>(2);
	int los = Los(shootingModel, target);
	CLUAScript::SetArgument(los);
	return 1;
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

int ShowMessageBox(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() < 1 || CLUAScript::GetArgumentCount() > 2)
        return luaL_error(L, "1 or 2 argument expected (caption, text)");
	char* text =  CLUAScript::GetArgument<char*>(1);
	char* caption = "";
	if(CLUAScript::GetArgumentCount() == 2)
		caption = CLUAScript::GetArgument<char*>(2);
	MessageBoxA(NULL, text, caption,0);
	return 0;
}

int RunScript(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (filename)");
	char* filename =  CLUAScript::GetArgument<char*>(1);
	CLUAScript::RunScript(filename);
	return 0;
}

int GetGlobalProperty(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (key)");
	char* key = CLUAScript::GetArgument<char*>(1);
	CLUAScript::SetArgument(CGameModel::GetIntanse().lock()->GetProperty(key).c_str());
	return 1;
}

int SetGlobalProperty(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 arguments expected (key, value)");
	char* key = CLUAScript::GetArgument<char*>(1);
	char* value = CLUAScript::GetArgument<char*>(2);
	CGameModel::GetIntanse().lock()->SetProperty(key, value);
	return 0;
}

int IncludeLibrary(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (libName)");
	char* lib = CLUAScript::GetArgument<char*>(1);
	CLUAScript::IncludeLibrary(lib);
	return 0;
}

int SetSelectionCallback(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (funcName)");
	std::string func = CLUAScript::GetArgument<char*>(1);
	std::function<void()> function;
	if(!func.empty())
	{
		function = [func]()
		{ 
			CLUAScript::CallFunction(func);
		};
	}
	CGameView::GetIntanse().lock()->SetSelectionCallback(function);
	return 0;
}

int SetUpdateCallback(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (funcName)");
	std::string func = CLUAScript::GetArgument<char*>(1);
	std::function<void()> function;
	if(!func.empty())
	{
		function = [func]()
		{ 
			CLUAScript::CallFunction(func);
		};
	}
	CGameView::GetIntanse().lock()->SetUpdateCallback(function);
	return 0;
}

int SetTimedCallback(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 3)
		return luaL_error(L, "3 argument expected (funcName, time, repeat)");
	std::string func = CLUAScript::GetArgument<char*>(1);
	unsigned int time = CLUAScript::GetArgument<unsigned int>(2);
	bool repeat = CLUAScript::GetArgument<bool>(3);
	unsigned int index = CTimedCallback::GetInstance()->AddCallback(func, time, repeat);
	CLUAScript::SetArgument((int)index);
	return 1;
}

int DeleteTimedCallback(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (ID)");
	unsigned int id = CLUAScript::GetArgument<unsigned int>(1);
	CTimedCallback::GetInstance()->DeleteCallback(id);
	return 0;
}

int ResizeWindow(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 argument expected (height, width)");
	int height = CLUAScript::GetArgument<int>(1);
	int width = CLUAScript::GetArgument<int>(2);
	CGameView::GetIntanse().lock()->ResizeWindow(height, width);
	return 0;
}

int BindKey(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 5)
		return luaL_error(L, "5 argument expected (key, shift, ctrl, alt, funcName)");
	unsigned char key = CLUAScript::GetArgument<unsigned int>(1);
	bool shift = CLUAScript::GetArgument<bool>(2);
	bool ctrl = CLUAScript::GetArgument<bool>(3);
	bool alt = CLUAScript::GetArgument<bool>(4);
	std::string func = CLUAScript::GetArgument<char*>(5);
	std::function<void()> function;
	if(!func.empty())
	{
		function = [func]()
		{ 
			CLUAScript::CallFunction(func);
		};
	}
	CInput::BindKey(key, shift, ctrl, alt, function);
	return 0;
}

int SetShaders(lua_State* L)
{
	int n = CLUAScript::GetArgumentCount();
	std::string vertex, fragment, geometry;
	if(n > 0) vertex = CLUAScript::GetArgument<const char*>(1);
	if(n > 1) fragment = CLUAScript::GetArgument<const char*>(2);
	if(n > 2) geometry = CLUAScript::GetArgument<const char*>(3);
	if(n > 3) return luaL_error(L, "up to 3 argument expected (vertex shader, fragment shader, geometry shader)");
	CGameView::GetIntanse().lock()->NewShaderProgram(vertex, fragment, geometry);
	return 0;
}

int EnableLight(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (index)");
	int i = CLUAScript::GetArgument<int>(1) - 1;
	glEnable(GL_LIGHT0 + i);
	return 0;
}

int DisableLight(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (index)");
	int i = CLUAScript::GetArgument<int>(1) - 1;
	glDisable(GL_LIGHT0 + i);
	return 0;
}

int SetLightPosition(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 4)
		return luaL_error(L, "4 argument expected (index, x, y, z)");
	int i = CLUAScript::GetArgument<int>(1) - 1;
	float pos[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	pos[0] = CLUAScript::GetArgument<float>(2);
	pos[1] = CLUAScript::GetArgument<float>(3);
	pos[2] = CLUAScript::GetArgument<float>(4);
	CGameView::GetIntanse().lock()->SetLightPosition(i, pos);
	return 0;
}

int SetLightAmbient(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 5)
		return luaL_error(L, "5 argument expected (index, r, g, b, a)");
	int i = CLUAScript::GetArgument<int>(1) -1;
	float color[4];
	color[0] = CLUAScript::GetArgument<float>(2);
	color[1] = CLUAScript::GetArgument<float>(3);
	color[2] = CLUAScript::GetArgument<float>(4);
	color[3] = CLUAScript::GetArgument<float>(5);
	glLightfv(GL_LIGHT0 + i, GL_AMBIENT, color);
	return 0;
}

int SetLightDiffuse(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 5)
		return luaL_error(L, "5 argument expected (index, r, g, b, a)");
	int i = CLUAScript::GetArgument<int>(1) - 1;
	float color[4];
	color[0] = CLUAScript::GetArgument<float>(2);
	color[1] = CLUAScript::GetArgument<float>(3);
	color[2] = CLUAScript::GetArgument<float>(4);
	color[3] = CLUAScript::GetArgument<float>(5);
	glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, color);
	return 0;
}

int SetLightSpecular(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 5)
		return luaL_error(L, "5 argument expected (index, r, g, b, a)");
	int i = CLUAScript::GetArgument<int>(1) - 1;
	float color[4];
	color[0] = CLUAScript::GetArgument<float>(2);
	color[1] = CLUAScript::GetArgument<float>(3);
	color[2] = CLUAScript::GetArgument<float>(4);
	color[3] = CLUAScript::GetArgument<float>(5);
	glLightfv(GL_LIGHT0 + i, GL_SPECULAR, color);
	return 0;
}

int EnableVertexLightning(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetIntanse().lock()->EnableVertexLightning();
	return 0;
}

int DisableVertexLightning(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetIntanse().lock()->DisableVertexLightning();
	return 0;
}

int EnableShadowMap(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 arguments (shadowMap size, shadow max Angle) expected");
	int size = CLUAScript::GetArgument<int>(1);
	float angle = CLUAScript::GetArgument<float>(2);
	CGameView::GetIntanse().lock()->EnableShadowMap(size, angle);
	return 0;
}

int DisableShadowMap(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetIntanse().lock()->DisableShadowMap();
	return 0;
}

int BeginActionCompound(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CCommandHandler::GetInstance().lock()->BeginCompound();
	return 0;
}

int EndActionCompound(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CCommandHandler::GetInstance().lock()->EndCompound();
	return 0;
}


void RegisterFunctions(CLUAScript & lua)
{
	lua.RegisterConstant(CreateTable, "CreateTable");
	lua.RegisterConstant(CreateSkybox, "CreateSkybox");
	lua.RegisterConstant(CameraSetLimits, "CameraSetLimits");
	lua.RegisterConstant(Ruler, "Ruler");
	lua.RegisterConstant(Undo, "Undo");
	lua.RegisterConstant(Redo, "Redo");
	lua.RegisterConstant(ShowMessageBox, "MessageBox");
	lua.RegisterConstant(RunScript, "RunScript");
	lua.RegisterConstant(GetGlobalProperty, "GetGlobalProperty");
	lua.RegisterConstant(SetGlobalProperty, "SetGlobalProperty");
	lua.RegisterConstant(IncludeLibrary, "IncludeLibrary");
	lua.RegisterConstant(SetSelectionCallback, "SetSelectionCallback");
	lua.RegisterConstant(SetUpdateCallback, "SetUpdateCallback");
	lua.RegisterConstant(SetTimedCallback, "SetTimedCallback");
	lua.RegisterConstant(DeleteTimedCallback, "DeleteTimedCallback");
	lua.RegisterConstant(LoS, "LoS");
	lua.RegisterConstant(ResizeWindow, "ResizeWindow");
	lua.RegisterConstant(BindKey, "BindKey");
	lua.RegisterConstant(SetShaders, "SetShaders");
	lua.RegisterConstant(EnableLight, "EnableLight");
	lua.RegisterConstant(DisableLight, "DisableLight");
	lua.RegisterConstant(SetLightPosition, "SetLightPosition");
	lua.RegisterConstant(SetLightAmbient, "SetLightAmbient");
	lua.RegisterConstant(SetLightDiffuse, "SetLightDiffuse");
	lua.RegisterConstant(SetLightSpecular, "SetLightSpecular");
	lua.RegisterConstant(EnableVertexLightning, "EnableVertexLightning");
	lua.RegisterConstant(DisableVertexLightning, "DisableVertexLightning");
	lua.RegisterConstant(EnableShadowMap, "EnableShadowMap");
	lua.RegisterConstant(DisableShadowMap, "DisableShadowMap");
	lua.RegisterConstant(BeginActionCompound, "BeginActionCompound");
	lua.RegisterConstant(EndActionCompound, "EndActionCompound");
}