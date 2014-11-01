#include "LUAScriptHandler.h"
#include "../view/GameView.h"
#include "../controller/GameController.h"
#include "../view/Input.h"
#include "../LogWriter.h"
#include <GL/glut.h>
#include "TimedCallback.h"
#include "../OSSpecific.h"
#include "../Network.h"

int CreateTable(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
        return luaL_error(L, "3 argument expected (width, height, texture)");
	float width = CLUAScript::GetArgument<float>(1);
	float height = CLUAScript::GetArgument<float>(2);
	std::string texture = CLUAScript::GetArgument<char*>(3);
	CGameView::GetInstance().lock()->CreateTable(width, height, texture);
	return 0;
}

int CreateSkybox(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "2 argument expected (size, texture folder)");
	double size = CLUAScript::GetArgument<double>(1);
	std::string texture = CLUAScript::GetArgument<char*>(2);
	CGameView::GetInstance().lock()->CreateSkybox(size, texture);
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
	CGameView::GetInstance().lock()->GetCamera()->SetLimits(maxTransX, maxTransY, maxScale, minScale);
	return 0;
}

int LoS(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "2 argument expected (source, target)");
	IObject* shootingModel = (IObject*)CLUAScript::GetArgument<void*>(1);
	IObject* target = (IObject*)CLUAScript::GetArgument<void*>(2);
	int los = CGameController::GetInstance().lock()->GetLineOfSight(shootingModel, target);
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

int ShMessageBox(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() < 1 || CLUAScript::GetArgumentCount() > 2)
        return luaL_error(L, "1 or 2 argument expected (text, caption)");
	std::string text =  CLUAScript::GetArgument<char*>(1);
	std::string caption = "";
	if(CLUAScript::GetArgumentCount() == 2)
		caption = CLUAScript::GetArgument<char*>(2);
	ShowMessageBox(text, caption);
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
	CLUAScript::SetArgument(CGameModel::GetInstance().lock()->GetProperty(key).c_str());
	return 1;
}

int SetGlobalProperty(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 arguments expected (key, value)");
	char* key = CLUAScript::GetArgument<char*>(1);
	char* value = CLUAScript::GetArgument<char*>(2);
	CGameModel::GetInstance().lock()->SetProperty(key, value);
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
	CGameController::GetInstance().lock()->SetSelectionCallback(function);
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
	CGameView::GetInstance().lock()->SetUpdateCallback(function);
	return 0;
}

int SetOnStateRecievedCallback(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (funcName)");
	std::string func = CLUAScript::GetArgument<char*>(1);
	std::function<void()> function;
	if (!func.empty())
	{
		function = [func]()
		{
			CLUAScript::CallFunction(func);
		};
	}
	CNetwork::GetInstance().lock()->SetStateRecievedCallback(function);
	return 0;
}

int SetOnStringRecievedCallback(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (funcName)");
	std::string func = CLUAScript::GetArgument<char*>(1);
	std::function<void(const char*)> function;
	if (!func.empty())
	{
		function = [func](const char* param)
		{
			CLUAScript::CallFunction(func, param);
		};
	}
	CNetwork::GetInstance().lock()->SetStringRecievedCallback(function);
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

int SetLMBCallback(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 argument expected (function name, disable default behavior)");
	std::string func = CLUAScript::GetArgument<char*>(1);
	bool disable = CLUAScript::GetArgument<bool>(2);
	CInput::SetLMBCallback(func, disable);
	return 0;
}

int SetRMBCallback(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 argument expected (function name, disable default behavior)");
	std::string func = CLUAScript::GetArgument<char*>(1);
	bool disable = CLUAScript::GetArgument<bool>(2);
	CInput::SetRMBCallback(func, disable);
	return 0;
}

int ResizeWindow(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 argument expected (height, width)");
	int height = CLUAScript::GetArgument<int>(1);
	int width = CLUAScript::GetArgument<int>(2);
	CGameView::GetInstance().lock()->ResizeWindow(height, width);
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
	CGameView::GetInstance().lock()->NewShaderProgram(vertex, fragment, geometry);
	return 0;
}

int EnableLight(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (index)");
	int i = CLUAScript::GetArgument<int>(1) - 1;
	if (i < 0 || i > 7) return luaL_error(L, "only 8 light sources are supported");
	glEnable(GL_LIGHT0 + i);
	return 0;
}

int DisableLight(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (index)");
	int i = CLUAScript::GetArgument<int>(1) - 1;
	if (i < 0 || i > 7) return luaL_error(L, "only 8 light sources are supported");
	glDisable(GL_LIGHT0 + i);
	return 0;
}

int SetLightPosition(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 4)
		return luaL_error(L, "4 argument expected (index, x, y, z)");
	int i = CLUAScript::GetArgument<int>(1) - 1;
	if (i < 0 || i > 7) return luaL_error(L, "only 8 light sources are supported");
	float pos[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	pos[0] = CLUAScript::GetArgument<float>(2);
	pos[1] = CLUAScript::GetArgument<float>(3);
	pos[2] = CLUAScript::GetArgument<float>(4);
	CGameView::GetInstance().lock()->SetLightPosition(i, pos);
	return 0;
}

int SetLightAmbient(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 5)
		return luaL_error(L, "5 argument expected (index, r, g, b, a)");
	int i = CLUAScript::GetArgument<int>(1) -1;
	if (i < 0 || i > 7) return luaL_error(L, "only 8 light sources are supported");
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
	if (i < 0 || i > 7) return luaL_error(L, "only 8 light sources are supported");
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
	if (i < 0 || i > 7) return luaL_error(L, "only 8 light sources are supported");
	float color[4];
	color[0] = CLUAScript::GetArgument<float>(2);
	color[1] = CLUAScript::GetArgument<float>(3);
	color[2] = CLUAScript::GetArgument<float>(4);
	color[3] = CLUAScript::GetArgument<float>(5);
	glLightfv(GL_LIGHT0 + i, GL_SPECULAR, color);
	return 0;
}

int EnableVertexLighting(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetInstance().lock()->EnableVertexLightning();
	return 0;
}

int DisableVertexLighting(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetInstance().lock()->DisableVertexLightning();
	return 0;
}

int EnableShadowMap(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 arguments (shadowMap size, shadow max Angle) expected");
	int size = CLUAScript::GetArgument<int>(1);
	float angle = CLUAScript::GetArgument<float>(2);
	CGameView::GetInstance().lock()->EnableShadowMap(size, angle);
	return 0;
}

int DisableShadowMap(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetInstance().lock()->DisableShadowMap();
	return 0;
}

int EnableMSAA(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetInstance().lock()->EnableMSAA();
	return 0;
}

int DisableMSAA(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetInstance().lock()->DisableMSAA();
	return 0;
}

int SetAnisotropy(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected");
	float a = CLUAScript::GetArgument<float>(1);
	CGameView::GetInstance().lock()->SetAnisotropy(a);
	return 0;
}

int GetMaxAnisotropy(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CLUAScript::SetArgument<double>(CGameView::GetMaxAnisotropy());
	return 1;
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

int NetHost(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (port)");
	int port = CLUAScript::GetArgument<int>(1);
	CNetwork::GetInstance().lock()->Host(port);
	return 0;
}

int NetClient(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 argument expected (ip, port)");
	std::string ip = CLUAScript::GetArgument<const char*>(1);
	int port = CLUAScript::GetArgument<int>(2);
	CNetwork::GetInstance().lock()->Client(ip.c_str(), port);
	return 0;
}

int NetSendMessage(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 arguments expected (message)");
	std::string message = CLUAScript::GetArgument<const char*>(1);
	CNetwork::GetInstance().lock()->SendMessag(message);
	return 0;
}

int SaveGame(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 arguments expected (filename)");
	std::string path = CLUAScript::GetArgument<const char*>(1);
	CGameView::GetInstance().lock()->Save(path);
	return 0;
}

int LoadGame(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 arguments expected (filename)");
	std::string path = CLUAScript::GetArgument<const char*>(1);
	CGameView::GetInstance().lock()->Load(path);
	return 0;
}

int ClearResources(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetInstance().lock()->ClearResources();
	return 0;
}

int SetWindowTitle(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 arguments expected (title)");
	std::string title = CLUAScript::GetArgument<const char*>(1);
	CGameView::GetInstance().lock()->SetWindowTitle(title);
	return 0;
}

int Preload(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (image path)");
	std::string image = CLUAScript::GetArgument<const char*>(1);
	CGameView::GetInstance().lock()->Preload(image);
	return 0;
}

int PreloadModel(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (model name)");
	std::string model = CLUAScript::GetArgument<const char*>(1);
	CGameView::GetInstance().lock()->GetModelManager()->LoadIfNotExist(model);
	return 0;
}

int LoadModuleFile(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
		return luaL_error(L, "1 argument expected (module file)");
	std::string module = CLUAScript::GetArgument<const char*>(1);
	CGameView::GetInstance().lock()->LoadModule(module);
	return 0;
}

int GetFilesList(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
		return luaL_error(L, "3 arguments expected (path, mask, recursive)");
	std::string path = CLUAScript::GetArgument<const char*>(1);
	std::string mask = CLUAScript::GetArgument<const char*>(2);
	bool recursive = CLUAScript::GetArgument<bool>(3);
	std::vector<std::string> files = GetFiles(path, mask, recursive);
	CLUAScript::SetArray(files);
	return 1;
}

int Uniform1i(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 arguments expected (uniform name, value)");
	std::string name = CLUAScript::GetArgument<const char*>(1);
	int value = CLUAScript::GetArgument<int>(2);
	CGameView::GetInstance().lock()->GetShaderManager()->SetUniformValue(name, value);
	return 0;
}

int Uniform1f(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "2 arguments expected (uniform name, value)");
	std::string name = CLUAScript::GetArgument<const char*>(1);
	float value = CLUAScript::GetArgument<float>(2);
	CGameView::GetInstance().lock()->GetShaderManager()->SetUniformValue(name, value);
	return 0;
}

int Uniform1fv(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
		return luaL_error(L, "3 arguments expected (uniform name, values count, values array)");
	std::string name = CLUAScript::GetArgument<const char*>(1);
	unsigned int count = CLUAScript::GetArgument<unsigned int>(2);
	std::vector<float> value = CLUAScript::GetArray<float>(3);
	if (value.size() < count) return luaL_error(L, "Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager()->SetUniformValue(name, count, &value[0]);
	return 0;
}

int Uniform2fv(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
		return luaL_error(L, "3 arguments expected (uniform name, values count, values array)");
	std::string name = CLUAScript::GetArgument<const char*>(1);
	unsigned int count = CLUAScript::GetArgument<unsigned int>(2);
	std::vector<float> value = CLUAScript::GetArray<float>(3);
	if (value.size() < count * 2) return luaL_error(L, "Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager()->SetUniformValue2(name, count, &value[0]);
	return 0;
}

int Uniform3fv(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
		return luaL_error(L, "3 arguments expected (uniform name, values count, values array)");
	std::string name = CLUAScript::GetArgument<const char*>(1);
	unsigned int count = CLUAScript::GetArgument<unsigned int>(2);
	std::vector<float> value = CLUAScript::GetArray<float>(3);
	if (value.size() < count * 3) return luaL_error(L, "Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager()->SetUniformValue3(name, count, &value[0]);
	return 0;
}

int Uniform4fv(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
		return luaL_error(L, "3 arguments expected (uniform name, values count, values array)");
	std::string name = CLUAScript::GetArgument<const char*>(1);
	unsigned int count = CLUAScript::GetArgument<unsigned int>(2);
	std::vector<float> value = CLUAScript::GetArray<float>(3);
	if (value.size() < count * 4) return luaL_error(L, "Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager()->SetUniformValue4(name, count, &value[0]);
	return 0;
}

int UniformMatrix4fv(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 3)
		return luaL_error(L, "3 arguments expected (uniform name, values count, values array)");
	std::string name = CLUAScript::GetArgument<const char*>(1);
	unsigned int count = CLUAScript::GetArgument<unsigned int>(2);
	std::vector<float> value = CLUAScript::GetArray<float>(3);
	if (value.size() < count * 16) return luaL_error(L, "Not enough elements in the array");
	CGameView::GetInstance().lock()->GetShaderManager()->SetUniformMatrix4(name, count, &value[0]);
	return 0;
}

int EnableGPUSkinning(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetInstance().lock()->EnableGPUSkinning();
	return 0;
}

int DisableGPUSkinning(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 0)
		return luaL_error(L, "no arguments expected");
	CGameView::GetInstance().lock()->DisableGPUSkinning();
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
	lua.RegisterConstant(ShMessageBox, "MessageBox");
	lua.RegisterConstant(RunScript, "RunScript");
	lua.RegisterConstant(GetGlobalProperty, "GetGlobalProperty");
	lua.RegisterConstant(SetGlobalProperty, "SetGlobalProperty");
	lua.RegisterConstant(IncludeLibrary, "IncludeLibrary");
	lua.RegisterConstant(SetSelectionCallback, "SetSelectionCallback");
	lua.RegisterConstant(SetUpdateCallback, "SetUpdateCallback");
	lua.RegisterConstant(SetOnStateRecievedCallback, "SetOnStateRecievedCallback");
	lua.RegisterConstant(SetOnStringRecievedCallback, "SetOnStringRecievedCallback");
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
	lua.RegisterConstant(EnableVertexLighting, "EnableVertexLighting");
	lua.RegisterConstant(DisableVertexLighting, "DisableVertexLighting");
	lua.RegisterConstant(EnableShadowMap, "EnableShadowMap");
	lua.RegisterConstant(DisableShadowMap, "DisableShadowMap");
	lua.RegisterConstant(EnableMSAA, "EnableMSAA");
	lua.RegisterConstant(DisableMSAA, "DisableMSAA");
	lua.RegisterConstant(SetAnisotropy, "SetAnisotropy");
	lua.RegisterConstant(GetMaxAnisotropy, "GetMaxAnisotropy");
	lua.RegisterConstant(BeginActionCompound, "BeginActionCompound");
	lua.RegisterConstant(EndActionCompound, "EndActionCompound");
	lua.RegisterConstant(NetHost, "NetHost");
	lua.RegisterConstant(NetClient, "NetClient");
	lua.RegisterConstant(NetSendMessage, "NetSendMessage");
	lua.RegisterConstant(SaveGame, "SaveGame");
	lua.RegisterConstant(LoadGame, "LoadGame");
	lua.RegisterConstant(ClearResources, "ClearResources");
	lua.RegisterConstant(SetWindowTitle, "SetWindowTitle");
	lua.RegisterConstant(Preload, "Preload");
	lua.RegisterConstant(PreloadModel, "PreloadModel");
	lua.RegisterConstant(LoadModuleFile, "LoadModule");
	lua.RegisterConstant(GetFilesList, "GetFilesList");
	lua.RegisterConstant(Uniform1i, "Uniform1i");
	lua.RegisterConstant(Uniform1f, "Uniform1f");
	lua.RegisterConstant(Uniform1fv, "Uniform1fv");
	lua.RegisterConstant(Uniform2fv, "Uniform2fv");
	lua.RegisterConstant(Uniform3fv, "Uniform3fv");
	lua.RegisterConstant(Uniform4fv, "Uniform4fv");
	lua.RegisterConstant(UniformMatrix4fv, "UniformMatrix4fv");
	lua.RegisterConstant(SetLMBCallback, "SetLMBCallback");
	lua.RegisterConstant(SetRMBCallback, "SetRMBCallback");
	lua.RegisterConstant(EnableGPUSkinning, "EnableGPUSkinning");
	lua.RegisterConstant(DisableGPUSkinning, "DisableGPUSkinning");
}