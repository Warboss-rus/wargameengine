#pragma once
#include "../controller/IScriptHandler.h"
#include <map>

struct lua_State;

class CScriptHandlerLua : public IScriptHandler
{
public:
	CScriptHandlerLua();
	~CScriptHandlerLua();

	void Reset() override;
	void RunScript(const Path& path) override;
	void CallFunction(const std::wstring& funcName, const FunctionArguments& arguments = FunctionArguments()) override;
	void RegisterConstant(const std::wstring& name, FunctionArgument const& value) override;
	void RegisterFunction(const std::wstring& name, FunctionHandler const& handler) override;
	void RegisterMethod(const std::wstring& className, const std::wstring& methodName, MethodHandler const& handler) override;
	void RegisterProperty(const std::wstring& className, const std::wstring& propertyName, SetterHandler const& setterHandler, GetterHandler const& getterHandler) override;
	void RegisterProperty(const std::wstring& className, const std::wstring& propertyName, GetterHandler const& getterHandler) override;

	static void* GetUserData(lua_State* L, int index);
	static void CallFunctionImpl(const FunctionArguments& arguments, lua_State* lua_state);

private:
	static int FunctionCallee(lua_State* L);
	static int MethodCallee(lua_State* L);
	static int IndexCallee(lua_State* L);
	static int NewIndexCallee(lua_State* L);
	static int luaError(lua_State* L);
	static int PushReturnValue(lua_State* L, FunctionArgument const& arg);
	static int NewClassInstance(lua_State* L, void* ptr, std::string const& className);

	void RegisterClass(std::string const& className);

	lua_State* m_lua_state;
	std::map<std::string, FunctionHandler> m_functions;
	struct sLuaClass
	{
		std::map<std::string, MethodHandler> methods;
		std::map<std::string, SetterHandler> setters;
		std::map<std::string, GetterHandler> getters;
	};
	static int GetClassAndInstance(lua_State* L, void** instance, sLuaClass** classPtr);
	std::map<std::string, sLuaClass> m_classes;
};
