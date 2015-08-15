#include <string>
#include <vector>
#include "../LUA/lua.hpp"
#pragma once

class CLUAScript
{
public:
	CLUAScript();
	CLUAScript(lua_State* state);
	~CLUAScript();
	static int RunScript(std::string const& file);
	static int GetArgumentCount();
	template<class T>
	static T GetArgument(int index);
	template<class T>
	static void SetArgument(T arg);
	template<class T>
	void RegisterConstant(T value, char* constantname);
	void RegisterClass(const luaL_Reg funcs[], std::string const& className);
	static void CallFunction(std::string const& funcName);
	static void CallFunctionFromReference(int referenceIndex);
	template<class X>
	static void CallFunction(std::string const& funcName, X param);
	static void CallFunction(std::string const& funcName, void* param, std::string const& className);
	static void CallFunction(std::string const& funcName, void* param, std::string const& className, double x, double y, double z);
	template<class U>
	static U CallFunctionReturn(std::string const& funcName);
	template<class U, class T>
	static U CallFunctionReturn(std::string const& funcName, T param);
	static void CallFunctionReturn4(std::string const& funcName, double x, double y, double z, double w, double & x1, double & y1, double & z1, double & w1);
	static void * GetClassInstance(std::string const& className);
	static int NewInstanceClass(void* instance, std::string const& className);
	template<class T>
	static std::vector<T> GetArray(int index);
	template<class T>
	static void SetArray(std::vector<T> arr);
	static std::string GetKeyForGetter();
	static bool IsClassInstance(int index = 1);
	static int StoreReference(int index = 1);
	static void LoadRefenrece(int reference);
	static bool IsString(int index);
	static bool IsNil(int index);
private:
	static lua_State* m_lua_state;
};