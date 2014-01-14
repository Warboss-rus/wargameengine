#include "LUAScriptHandler.h"
#include <exception>

lua_State* CLUAScript::m_lua_state;

int luaError( lua_State *L )
{
    const char* str = lua_tostring( L, -1 );
    lua_pop(L, 1);
	throw std::exception(str);
    return 0;
}

CLUAScript::CLUAScript()
{
	m_lua_state = luaL_newstate();
	lua_register( m_lua_state, "_ALERT", luaError );
	lua_atpanic(m_lua_state, luaError);
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
float CLUAScript::GetArgument<float>(int index)
{
    return luaL_checknumber(m_lua_state,index);
}

template<>
int CLUAScript::GetArgument<int>(int index)
{
    return luaL_checkinteger(m_lua_state,index);
}

template<>
unsigned int CLUAScript::GetArgument<unsigned int>(int index)
{
    return luaL_checkunsigned(m_lua_state,index);
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
void CLUAScript::SetArgument<float>(float arg)
{
	lua_pushnumber(m_lua_state, arg);
}

template<>
void CLUAScript::SetArgument<int>(int arg)
{
	lua_pushnumber(m_lua_state, arg);
}

template<>
void CLUAScript::SetArgument<const char*>(const char* arg)
{
	lua_pushstring(m_lua_state, arg);
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
		throw std::runtime_error(err);
	}
	return lua_tointeger(m_lua_state, lua_gettop(m_lua_state));
}

template<>
void CLUAScript::RegisterConstant<int>(int value, char* constantname)
{
    lua_pushinteger(m_lua_state, value);
    lua_setglobal(m_lua_state,constantname);
}

template<>
void CLUAScript::RegisterConstant<float>(float value, char* constantname)
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
	lua_pcall(m_lua_state, 0, 0, 0);
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
	luaL_checktype(m_lua_state, 1, LUA_TTABLE);
    
    lua_newtable(m_lua_state);      // Create table to represent instance

    // Set first argument of new to metatable of instance
    lua_pushvalue(m_lua_state,1);       
    lua_setmetatable(m_lua_state, -2);

    // Do function lookups in metatable
    lua_pushvalue(m_lua_state,1);
    lua_setfield(m_lua_state, 1, "__index");  

	void **s = (void **)lua_newuserdata(m_lua_state, sizeof(void *));

	*s = instance;
	luaL_getmetatable(m_lua_state, std::string("Classes." + className).c_str());
	lua_setmetatable(m_lua_state, -2);
	lua_setfield(m_lua_state, -2, "__self"); 
    
	return 1; 
}

void CLUAScript::IncludeLibrary(std::string const& libName)
{
	static const luaL_Reg lualibs[] = 
    {
		{"base", luaopen_base },
		{"bit32", luaopen_bit32 },
		{"coroutine", luaopen_coroutine },
		{"debug", luaopen_debug },
		{"io", luaopen_io },
		{"math", luaopen_math },
		{"os", luaopen_os },
		{"package", luaopen_package },
		{"string", luaopen_string },
		{"table", luaopen_table },
		{NULL, NULL}
    };

    for(const luaL_Reg *lib = lualibs; lib->func != NULL; lib++)
    {
        if(libName == lib->name)
		{
			luaL_requiref(m_lua_state, lib->name, lib->func, 1);
			lua_settop(m_lua_state, 0);
		}
    }
}