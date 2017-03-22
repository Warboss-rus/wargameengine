#pragma once
#include "../controller/IScriptHandler.h"
#include <map>

struct lua_State;

class CScriptHandlerLua : public IScriptHandler
{
public:
	CScriptHandlerLua();
	~CScriptHandlerLua();
	virtual void RunScript(std::wstring const& path) override;
	virtual void CallFunction(std::wstring const& funcName, FunctionArguments const& arguments = FunctionArguments()) override;
	virtual void RegisterConstant(std::wstring const& name, FunctionArgument const& value) override;
	virtual void RegisterFunction(std::wstring const& name, FunctionHandler const& handler) override;
	virtual void RegisterMethod(std::wstring const& className, std::wstring const& methodName, MethodHandler const& handler) override;
	virtual void RegisterProperty(std::wstring const& className, std::wstring const& propertyName, SetterHandler const& setterHandler, GetterHandler const& getterHandler) override;
	virtual void RegisterProperty(std::wstring const& className, std::wstring const& propertyName, GetterHandler const& getterHandler) override;
	static void* GetUserData(lua_State *L, int index);
	static void CallFunctionImpl(FunctionArguments const &arguments, lua_State* lua_state);
private:
	static int FunctionCallee(lua_State* L);
	static int MethodCallee(lua_State* L);
	static int IndexCallee(lua_State* L);
	static int NewIndexCallee(lua_State* L);
	static int luaError(lua_State *L);
	static int PushReturnValue(lua_State *L, FunctionArgument const& arg);
	static int NewClassInstance(lua_State *L, void* ptr, std::string const& className);
	void RegisterClass(std::string const& className);
	lua_State* m_lua_state;
	std::map<std::string, FunctionHandler> m_functions;
	struct sLuaClass
	{
		std::map<std::string, MethodHandler> methods;
		std::map<std::string, SetterHandler> setters;
		std::map<std::string, GetterHandler> getters;
	};
	static int GetClassAndInstance(lua_State *L, void ** instance, sLuaClass ** classPtr);
	std::map<std::string, sLuaClass> m_classes;
};
