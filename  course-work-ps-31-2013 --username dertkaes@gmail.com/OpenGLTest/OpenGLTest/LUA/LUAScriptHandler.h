#pragma comment(lib,"lua.lib")
extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}
#include <string>
#pragma once

class CLUAScript
{
public:
	CLUAScript();
	~CLUAScript();
	static int RunScript(std::string const& file);
	static int GetArgumentCount();
	template<class T>
	static T GetArgument(int index);
	template<class T>
	static void SetArgument(T arg);
	template<class T>
    static void RegisterConstant(T value, char* constantname);
	static void RegisterClass(const luaL_Reg funcs[], std::string const& className);
	static void CallFunction(std::string const& funcName);
	static void * GetClassInstance(std::string const& className);
	static int NewInstanceClass(void* instance, std::string const& className);
private:
	static lua_State* m_lua_state;
};