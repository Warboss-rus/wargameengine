#include "LUAScriptHandler.h"
#include <exception>

#include "../LogWriter.h"

lua_State* CLUAScript::m_lua_state;

int luaError( lua_State *L )
{
    std::string str = lua_tostring( L, -1 );
	LogWriter::WriteLine("LUA error:" + str);
    lua_pop(L, 1);
	throw std::exception();
}

CLUAScript::CLUAScript()
{
	m_lua_state = luaL_newstate();
	lua_register( m_lua_state, "_ALERT", luaError );
	lua_atpanic(m_lua_state, luaError);
	luaL_openlibs(m_lua_state);
}

CLUAScript::CLUAScript(lua_State* /*state*/)
//	:m_lua_state(state)
{
}

CLUAScript::~CLUAScript()
{
	lua_close(m_lua_state);
}

int CLUAScript::GetArgumentCount()
{
    return lua_gettop(m_lua_state);
}

template<>
double CLUAScript::GetArgument<double>(int index)
{
    return luaL_checknumber(m_lua_state,index);
}

template<>
float CLUAScript::GetArgument<float>(int index)
{
    return (float)luaL_checknumber(m_lua_state,index);
}

template<>
int CLUAScript::GetArgument<int>(int index)
{
    return static_cast<int>(luaL_checkinteger(m_lua_state,index));
}

template<>
unsigned int CLUAScript::GetArgument<unsigned int>(int index)
{
    return static_cast<unsigned int>(luaL_checkinteger(m_lua_state,index));
}

template<>
const char* CLUAScript::GetArgument<const char*>(int index)
{
	return luaL_checkstring(m_lua_state,index);
}

template<>
char* CLUAScript::GetArgument<char*>(int index)
{
	return (char*)luaL_checkstring(m_lua_state,index);
}

template<>
bool CLUAScript::GetArgument<bool>(int index)
{
	return lua_toboolean(m_lua_state,index) != 0;
}

template<>
void* CLUAScript::GetArgument<void*>(int index)
{
	luaL_checktype(m_lua_state, index, LUA_TTABLE); 
	lua_getfield(m_lua_state, index, "__self");
	
	void * ud = lua_touserdata(m_lua_state, -1);//return NULL
	return *((void**)ud);
}

template<>
void CLUAScript::SetArgument<double>(double arg)
{
	lua_pushnumber(m_lua_state, arg);
}

template<>
void CLUAScript::SetArgument<int>(int arg)
{
	lua_pushinteger(m_lua_state, arg);
}

template<>
void CLUAScript::SetArgument<const char*>(const char* arg)
{
	lua_pushstring(m_lua_state, arg);
}

template<>
void CLUAScript::SetArgument<const wchar_t*>(const wchar_t* arg)
{
	std::vector<char> str;
	size_t size = wcslen(arg);
	str.resize(size);
	wcstombs(str.data(), arg, size);
	str.push_back('\0');
	lua_pushstring(m_lua_state, str.data());
}

template<>
void CLUAScript::SetArgument<bool>(bool arg)
{
	lua_pushboolean(m_lua_state, arg);
}

int CLUAScript::RunScript(std::string const& file)
{
	int result = luaL_dofile(m_lua_state, file.c_str());
	if(result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
	return static_cast<int>(lua_tointeger(m_lua_state, lua_gettop(m_lua_state)));
}

template<>
void CLUAScript::RegisterConstant<int>(int value, char* constantname)
{
    lua_pushinteger(m_lua_state, value);
    lua_setglobal(m_lua_state,constantname);
}

template<>
void CLUAScript::RegisterConstant<double>(double value, char* constantname)
{
    lua_pushnumber(m_lua_state, value);
    lua_setglobal(m_lua_state,constantname);
}

template<>
void CLUAScript::RegisterConstant<char*>(char* value, char* constantname)
{
    lua_pushstring(m_lua_state, value);
    lua_setglobal(m_lua_state,constantname);
}

template<>
void CLUAScript::RegisterConstant<bool>(bool value, char* constantname)
{
    lua_pushboolean(m_lua_state, value);
    lua_setglobal(m_lua_state,constantname);
}

template<>
void CLUAScript::RegisterConstant<lua_CFunction>(lua_CFunction value, char* constantname)
{
    lua_pushcfunction(m_lua_state, value);
    lua_setglobal(m_lua_state,constantname);
}

void CLUAScript::RegisterClass(const luaL_Reg funcs[], std::string const& className)
{
	luaL_newmetatable(m_lua_state, std::string("Classes." + className).c_str());
	luaL_newlib( m_lua_state, funcs);
	lua_pushvalue(m_lua_state,-1);
    lua_setfield(m_lua_state, -2, "__index");
	lua_setglobal(m_lua_state, className.c_str());
}

void CLUAScript::CallFunction(std::string const& funcName)
{
	lua_getglobal(m_lua_state, funcName.c_str());
	int result = lua_pcall(m_lua_state, 0, 0, 0);
	if(result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

template<>
void CLUAScript::CallFunction<const char*>(std::string const& funcName, const char* param)
{
	lua_getglobal(m_lua_state, funcName.c_str());
	SetArgument(param);
	int result = lua_pcall(m_lua_state, 1, 0, 0);
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

void CLUAScript::CallFunction(std::string const& funcName, void* param, std::string const& className)
{
	lua_getglobal(m_lua_state, funcName.c_str());
	NewInstanceClass(param, className);
	int result = lua_pcall(m_lua_state, 1, 0, 0);
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

void CLUAScript::CallFunction(std::string const& funcName, void* param, std::string const& className, double x, double y, double z)
{
	lua_getglobal(m_lua_state, funcName.c_str());
	NewInstanceClass(param, className);
	SetArgument(x);
	SetArgument(y);
	SetArgument(z);
	int result = lua_pcall(m_lua_state, 4, 0, 0);
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

void CLUAScript::CallFunctionReturn4(std::string const& funcName, double x, double y, double z, double w, double & x1, double & y1, double & z1, double & w1)
{
	lua_getglobal(m_lua_state, funcName.c_str());
	SetArgument(x);
	SetArgument(y);
	SetArgument(z);
	SetArgument(w);
	int result = lua_pcall(m_lua_state, 4, 2, 0);
	x1 = GetArgument<double>(1);
	y1 = GetArgument<double>(2);
	z1 = GetArgument<double>(3);
	w1 = GetArgument<double>(4);
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

template<>
int CLUAScript::CallFunctionReturn<int>(std::string const& funcName)
{
	lua_getglobal(m_lua_state, funcName.c_str());
	int result = lua_pcall(m_lua_state, 0, 1, 0);
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
	result = GetArgument<int>(1);
	lua_pop(m_lua_state, 1);
	return result;
}

template<>
int CLUAScript::CallFunctionReturn<int, const char*>(std::string const& funcName, const char* param)
{
	lua_getglobal(m_lua_state, funcName.c_str());
	SetArgument(param);
	int result = lua_pcall(m_lua_state, 1, 1, 0);
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
	result = GetArgument<int>(1);
	lua_pop(m_lua_state, 1);
	return result;
}

void CLUAScript::CallFunctionFromReference(int referenceIndex)
{
	lua_rawgeti(m_lua_state, LUA_REGISTRYINDEX, referenceIndex);
	int result = lua_pcall(m_lua_state, 0, 0, 0);
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

void * CLUAScript::GetClassInstance(std::string const& className)
{
	void* ud = 0;
	luaL_checktype(m_lua_state, 1, LUA_TTABLE); 
	lua_getfield(m_lua_state, 1, "__self");
	
	ud = luaL_checkudata(m_lua_state, -1, std::string("Classes." + className).c_str());//return NULL
	return *((void**)ud);
}

int CLUAScript::NewInstanceClass(void* instance, std::string const& className)
{
	if (!instance)
	{
		lua_pushnil(m_lua_state);
		return 1;
	}
	
	luaL_checktype(m_lua_state, 1, LUA_TTABLE);
    
    lua_newtable(m_lua_state);      // Create table to represent instance. Stack:: table

    // Set first argument of new to metatable of instance
    lua_pushvalue(m_lua_state,1);       //Stack: table, 1
    lua_setmetatable(m_lua_state, -2); //stack: table

    // Do function lookups in metatable. Need to change for properties to work
    lua_pushvalue(m_lua_state,1); //Stack: table, 1
    lua_setfield(m_lua_state, 1, "__index");  //Stack: table

	void **s = (void **)lua_newuserdata(m_lua_state, sizeof(void *));//Stack: table, userdata

	*s = instance;
	luaL_getmetatable(m_lua_state, std::string("Classes." + className).c_str());//Stack: table, userdata, metatable

	lua_setmetatable(m_lua_state, -2);//Stack: table, userdata
	lua_setfield(m_lua_state, -2, "__self");//Stack: table
    
	return 1; 
}

template<>
std::vector<int> CLUAScript::GetArray<int>(int index)
{
	luaL_checktype(m_lua_state, index, LUA_TTABLE);
	int n = 0;
	std::vector<int> result;
	for (;;)
	{
		lua_rawgeti(m_lua_state, index, ++n);
		if (lua_isnil(m_lua_state, -1)) break;
		result.push_back(GetArgument<int>(-1));
		lua_pop(m_lua_state, 1);
	}
	lua_pop(m_lua_state, 1);
	return result;
}

template<>
std::vector<float> CLUAScript::GetArray<float>(int index)
{
	luaL_checktype(m_lua_state, index, LUA_TTABLE);
	int n = 0;
	std::vector<float> result;
	for (;;)
	{
		lua_rawgeti(m_lua_state, index, ++n);
		if (lua_isnil(m_lua_state, -1)) break;
		result.push_back(GetArgument<float>(-1));
		lua_pop(m_lua_state, 1);
	}
	lua_pop(m_lua_state, 1);
	return result;
}

template<>
std::vector<std::string> CLUAScript::GetArray<std::string>(int index)
{
	luaL_checktype(m_lua_state, index, LUA_TTABLE);
	int n = 0;
	std::vector<std::string> result;
	for (;;)
	{
		lua_rawgeti(m_lua_state, index, ++n);
		if (lua_isnil(m_lua_state, -1)) break;
		result.push_back(GetArgument<const char*>(-1));
		lua_pop(m_lua_state, 1);
	}
	lua_pop(m_lua_state, 1);
	return result;
}

template<>
void CLUAScript::SetArray<int>(std::vector<int> arr)
{
	lua_createtable(m_lua_state, arr.size(), 0);
	for (size_t i = 0; i < arr.size(); ++i)
	{
		lua_pushinteger(m_lua_state, arr[i]);
		lua_rawseti(m_lua_state, -2, i);
	}
}

template<>
void CLUAScript::SetArray<bool>(std::vector<bool> arr)
{
	lua_createtable(m_lua_state, arr.size(), 0);
	for (size_t i = 0; i < arr.size(); ++i)
	{
		lua_pushboolean(m_lua_state, arr[i]);
		lua_rawseti(m_lua_state, -2, i);
	}
}

template<>
void CLUAScript::SetArray<unsigned int>(std::vector<unsigned int> arr)
{
	lua_createtable(m_lua_state, arr.size(), 0);
	for (size_t i = 0; i < arr.size(); ++i)
	{
		lua_pushinteger(m_lua_state, arr[i]);
		lua_rawseti(m_lua_state, -2, i);
	}
}

template<>
void CLUAScript::SetArray<float>(std::vector<float> arr)
{
	lua_createtable(m_lua_state, arr.size(), 0);
	for (size_t i = 0; i < arr.size(); ++i)
	{
		lua_pushnumber(m_lua_state, arr[i]);
		lua_rawseti(m_lua_state, -2, i);
	}
}

template<>
void CLUAScript::SetArray<double>(std::vector<double> arr)
{
	lua_createtable(m_lua_state, arr.size(), 0);
	for (size_t i = 0; i < arr.size(); ++i)
	{
		lua_pushnumber(m_lua_state, arr[i]);
		lua_rawseti(m_lua_state, -2, i);
	}
}

template<>
void CLUAScript::SetArray<std::string>(std::vector<std::string> arr)
{
	lua_createtable(m_lua_state, arr.size(), 0);
	for (size_t i = 0; i < arr.size(); ++i) 
	{
		lua_pushstring(m_lua_state, arr[i].c_str());
		lua_rawseti(m_lua_state, -2, i+1);
	}
}

template<>
void CLUAScript::SetArray<const char*>(std::vector<const char*> arr)
{
	lua_createtable(m_lua_state, arr.size(), 0);
	for (size_t i = 0; i < arr.size(); ++i)
	{
		lua_pushstring(m_lua_state, arr[i]);
		lua_rawseti(m_lua_state, -2, i);
	}
}

std::string CLUAScript::GetKeyForGetter()
{
	const char* key = luaL_checkstring(m_lua_state, 2);
	lua_getmetatable(m_lua_state, 1);
	lua_getfield(m_lua_state, -1, key);

	// Either key is name of a method in the metatable
	if (!lua_isnil(m_lua_state, -1))
		return "";
	// ... or its a field access, so recall as self.get(self, value).
	lua_settop(m_lua_state, 2);

	return key;
}

bool CLUAScript::IsClassInstance(int index)
{
	luaL_checktype(m_lua_state, index, LUA_TTABLE);
	lua_getfield(m_lua_state, index, "__self");
	bool result = lua_isuserdata(m_lua_state, -1) != 0;
	lua_pop(m_lua_state, 1);
	return result;
}

int CLUAScript::StoreReference(int index)
{
	lua_pushvalue(m_lua_state, index);
	return luaL_ref(m_lua_state, LUA_REGISTRYINDEX);
}

void CLUAScript::LoadRefenrece(int reference)
{
	lua_rawgeti(m_lua_state, LUA_REGISTRYINDEX, reference);
}

bool CLUAScript::IsString(int index)
{
	return lua_isstring(m_lua_state, index) != 0;
}

bool CLUAScript::IsNil(int index)
{
	return lua_isnil(m_lua_state, index);
}