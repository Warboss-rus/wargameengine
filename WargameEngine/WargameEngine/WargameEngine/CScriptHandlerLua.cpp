#include "CScriptHandlerLua.h"
#include "..\LUA\lua.hpp"
#include "LogWriter.h"

class CLuaArguments : public IArguments
{
public:
	CLuaArguments(lua_State* lua_state, int diff = 1)
		:m_lua_state(lua_state), m_diff(diff)
	{
	}

	virtual int GetCount() const override
	{
		return lua_gettop(m_lua_state) - m_diff + 1;
	}

	virtual bool GetBool(int index) const override
	{
		return lua_toboolean(m_lua_state, index + m_diff) != 0;
	}

	virtual std::string GetStr(int index) const override
	{
		return luaL_checkstring(m_lua_state, index + m_diff);
	}

	virtual int GetInt(int index) const override
	{
		return static_cast<int>(luaL_checkinteger(m_lua_state, index + m_diff));
	}

	virtual double GetDbl(int index) const override
	{
		return luaL_checknumber(m_lua_state, index + m_diff);
	}

	virtual float GetFloat(int index) const override
	{
		return static_cast<float>(luaL_checknumber(m_lua_state, index + m_diff));
	}

	virtual void* GetClassInstance(int index) const override
	{
		return CScriptHandlerLua::GetUserData(m_lua_state, index + m_diff);
	}

	virtual std::vector<int> GetIntArray(int index) const override
	{
		luaL_checktype(m_lua_state, index + m_diff, LUA_TTABLE);
		int n = 0;
		std::vector<int> result;
		for (;;)
		{
			lua_rawgeti(m_lua_state, index + m_diff, ++n);
			if (lua_isnil(m_lua_state, -1)) break;
			result.push_back(GetInt(-1));
			lua_pop(m_lua_state, 1);
		}
		lua_pop(m_lua_state, 1);
		return result;
	}

	virtual std::vector<float> GetFloatArray(int index) const override
	{
		luaL_checktype(m_lua_state, index + m_diff, LUA_TTABLE);
		int n = 0;
		std::vector<float> result;
		for (;;)
		{
			lua_rawgeti(m_lua_state, index + m_diff, ++n);
			if (lua_isnil(m_lua_state, -1)) break;
			result.push_back(GetFloat(-1));
			lua_pop(m_lua_state, 1);
		}
		lua_pop(m_lua_state, 1);
		return result;
	}

	virtual std::vector<std::string> GetStrArray(int index) const override
	{
		luaL_checktype(m_lua_state, index + m_diff, LUA_TTABLE);
		int n = 0;
		std::vector<std::string> result;
		for (;;)
		{
			lua_rawgeti(m_lua_state, index + m_diff, ++n);
			if (lua_isnil(m_lua_state, -1)) break;
			result.push_back(GetStr(-1));
			lua_pop(m_lua_state, 1);
		}
		lua_pop(m_lua_state, 1);
		return result;
	}

	virtual bool IsBool(int index) const override
	{
		return lua_isboolean(m_lua_state, index + m_diff);
	}

	virtual bool IsStr(int index) const override
	{
		return lua_isstring(m_lua_state, index + m_diff) != 0;
	}

	virtual bool IsNumber(int index) const override
	{
		return lua_isnumber(m_lua_state, index + m_diff) != 0;
	}

	virtual bool IsClassInstance(int index) const override
	{
		luaL_checktype(m_lua_state, index + m_diff, LUA_TTABLE);
		lua_pushstring(m_lua_state, "__self");
		lua_rawget(m_lua_state, index + m_diff);
		bool result = lua_isuserdata(m_lua_state, -1) != 0;
		lua_pop(m_lua_state, 1);
		return result;
	}

	virtual bool IsIntArray(int index) const override
	{
		return lua_istable(m_lua_state, index + m_diff);
	}

	virtual bool IsFloatArray(int index) const override
	{
		return lua_istable(m_lua_state, index + m_diff);
	}

	virtual bool IsStrArray(int index) const override
	{
		return lua_istable(m_lua_state, index + m_diff);
	}
private:
	lua_State* m_lua_state;
	int m_diff;
};

int CScriptHandlerLua::luaError(lua_State *L)
{
	std::string str = lua_tostring(L, -1);
	LogWriter::WriteLine("LUA error:" + str);
	lua_pop(L, 1);
	throw std::runtime_error(str);
}

int CScriptHandlerLua::PushReturnValue(lua_State *L, FunctionArgument const& arg)
{
	switch (arg.type)
	{
	case FunctionArgument::Type::BOOLEAN:
		lua_pushboolean(L, *static_cast<bool*>(arg.data.get()));
		break;
	case FunctionArgument::Type::INT:
		lua_pushinteger(L, *static_cast<int*>(arg.data.get()));
		break;
	case FunctionArgument::Type::FLOAT:
		lua_pushnumber(L, *static_cast<float*>(arg.data.get()));
		break;
	case FunctionArgument::Type::DOUBLE:
		lua_pushnumber(L, *static_cast<double*>(arg.data.get()));
		break;
	case FunctionArgument::Type::STRING:
		lua_pushstring(L, static_cast<std::string*>(arg.data.get())->c_str());
		break;
	//TODO: arrays
	case FunctionArgument::Type::CLASS_INSTANCE:
	{
		auto inst = static_cast<FunctionArgument::sClassInstance*>(arg.data.get());
		CScriptHandlerLua::NewClassInstance(L, inst->ptr, inst->className);
	}break;
	default:
		lua_pushnil(L);
		break;
	}
	return arg.type == FunctionArgument::Type::TNULL ? 0 : 1;
}

CScriptHandlerLua::CScriptHandlerLua()
{
	m_lua_state = luaL_newstate();
	lua_register(m_lua_state, "_ALERT", luaError);
	lua_atpanic(m_lua_state, luaError);
	luaL_openlibs(m_lua_state);
}

CScriptHandlerLua::~CScriptHandlerLua()
{
	lua_close(m_lua_state);
}

void CScriptHandlerLua::RunScript(std::string const& path)
{
	int result = luaL_dofile(m_lua_state, path.c_str());
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

void CScriptHandlerLua::CallFunction(std::string const& funcName, FunctionArgument const& argument)
{
	lua_getglobal(m_lua_state, funcName.c_str());
	int argsCount = PushReturnValue(m_lua_state, argument);
	int result = lua_pcall(m_lua_state, argsCount, 0, 0);
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char *err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

void CScriptHandlerLua::RegisterConstant(std::string const& name, std::string const& value)
{
	lua_pushstring(m_lua_state, value.c_str());
	lua_setglobal(m_lua_state, name.c_str());
}

void CScriptHandlerLua::RegisterFunction(std::string const& name, FunctionHandler const& handler)
{
	m_functions.emplace(name, handler);
	lua_pushlightuserdata(m_lua_state, this);//Stack: userdata
	lua_pushstring(m_lua_state, name.c_str());//Stack: userdata, string
	lua_pushcclosure(m_lua_state, &FunctionCallee, 2);//Stack: CFunction
	lua_setglobal(m_lua_state, name.c_str());//Stack:
}

void CScriptHandlerLua::RegisterMethod(std::string const& className, std::string const& methodName, MethodHandler const& handler)
{
	if (m_classes.find(className) == m_classes.end())
	{
		RegisterClass(className);
	}
	auto& cl = m_classes[className];
	cl.methods.emplace(methodName, handler);

	lua_getglobal(m_lua_state, className.c_str());//Stack: metatable
	int index = lua_gettop(m_lua_state);
	lua_pushstring(m_lua_state, methodName.c_str());//Stack: metatable, string
	lua_pushlightuserdata(m_lua_state, this);//Stack: metatable, string, userdata
	lua_pushstring(m_lua_state, className.c_str());//Stack: metatable, string, userdata, string
	lua_pushstring(m_lua_state, methodName.c_str());//Stack: metatable, string, userdata, string, string
	lua_pushcclosure(m_lua_state, &MethodCallee, 3);//Stack: metatable, string, CFunction
	lua_rawset(m_lua_state, index);//Stack: metatable
	lua_pop(m_lua_state, 1);
}

void CScriptHandlerLua::RegisterProperty(std::string const& className, std::string const& propertyName, SetterHandler const& setterHandler, GetterHandler const& getterHandler)
{
	if (m_classes.find(className) == m_classes.end())
	{
		RegisterClass(className);
	}
	auto& cl = m_classes[className];
	cl.getters.emplace(propertyName, getterHandler);
	cl.setters.emplace(propertyName, setterHandler);
}

int CScriptHandlerLua::FunctionCallee(lua_State* L)
{
	auto ptr = static_cast<CScriptHandlerLua*>(lua_touserdata(L, lua_upvalueindex(1)));
	if (!ptr)
	{
		return luaL_error(L, "Cannot get handler instance. Stack is probably corrupted.");
	}
	std::string funcName = lua_tostring(L, lua_upvalueindex(2));
	CLuaArguments args(L);
	auto func = ptr->m_functions.find(funcName);
	if (func == ptr->m_functions.end() || !func->second)
	{
		return luaL_error(L, ("Handler for function " + funcName + " is not found").c_str());
	}
	try
	{
		return PushReturnValue(L, func->second(args));
	}
	catch (std::exception const& e)
	{
		return luaL_error(L, e.what());
	}
}

std::string DumpStack(lua_State* L)
{
	std::string result;
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		result += lua_typename(L, t);
		if (t == LUA_TTABLE)
		{
			for (int j = 0;;j++)
			{
				lua_rawgeti(L, i, j);
				int t = lua_type(L, -1);
				result += " ";
				result += lua_typename(L, t);
				lua_pop(L, 1);
				if (t == LUA_TNIL) break;
			}
		}
		result += '\n';
	}
	return result;
}

int CScriptHandlerLua::MethodCallee(lua_State* L)
{
	auto stack = DumpStack(L);
	void* instance;
	sLuaClass* cl;
	int result = GetClassAndInstance(L, &instance, &cl);
	if (result) return result;

	std::string methodName = lua_tostring(L, lua_upvalueindex(3));
	auto method = cl->methods.find(methodName);
	if (method != cl->methods.end() && method->second)
	{
		try
		{
			CLuaArguments args(L, 2);
			return PushReturnValue(L, method->second(instance, args));
		}
		catch (std::exception const& e)
		{
			return luaL_error(L, e.what());
		}
	}
	else
	{
		return luaL_error(L, ("Cannot find method " + methodName + " of class " + lua_tostring(L, lua_upvalueindex(2))).c_str());
	}
}

int CScriptHandlerLua::IndexCallee(lua_State* L)
{
	void* instance;
	sLuaClass* cl;
	int result = GetClassAndInstance(L, &instance, &cl);
	if (result) return result;

	std::string propertyName = luaL_checkstring(L, 2);
	auto prop = cl->getters.find(propertyName);
	if (prop != cl->getters.end() && prop->second)
	{
		try
		{
			return PushReturnValue(L, prop->second(instance));
		}
		catch (std::exception const& e)
		{
			return luaL_error(L, e.what());
		}
	}
	else
	{
		return luaL_error(L, ("Cannot find method or property " + propertyName + " of class " + lua_tostring(L, lua_upvalueindex(2))).c_str());
	}

	return 0;
}

int CScriptHandlerLua::NewIndexCallee(lua_State* L)
{
	void* instance;
	sLuaClass* cl;
	int result = GetClassAndInstance(L, &instance, &cl);
	if (result) return result;

	std::string propertyName = luaL_checkstring(L, 2);
	CLuaArguments args(L, 3);
	auto setter = cl->setters.find(propertyName);
	if (setter != cl->setters.end() && setter->second)
	{
		setter->second(instance, args);
		return 0;
	}
	else
	{
		return luaL_error(L, ("Cannot find setter for " + propertyName + " of class " + lua_tostring(L, lua_upvalueindex(2))).c_str());
	}
}

void CScriptHandlerLua::RegisterClass(std::string const& className)
{
	luaL_newmetatable(m_lua_state, ("Classes." + className).c_str());//Stack: metatable
	lua_pushlightuserdata(m_lua_state, this);//Stack: metatable, userdata
	lua_pushstring(m_lua_state, className.c_str());//Stack: metatable, userdata, string
	lua_pushcclosure(m_lua_state, &NewIndexCallee, 2);//Stack: metatable, CFunction
	lua_setfield(m_lua_state, -2, "__newindex");//Stack: metatable
	lua_pushvalue(m_lua_state, -1);//Stack: metatable, metatable
	lua_setglobal(m_lua_state, className.c_str());//Stack: metatable

	//this calls IndexCallee, but the table has no __self field
	luaL_newmetatable(m_lua_state, ("ClassesIndex." + className).c_str());//Stack: metatable, metatable
	lua_pushlightuserdata(m_lua_state, this);//Stack: metatable, metatable, userdata
	lua_pushstring(m_lua_state, className.c_str());//Stack: metatable, metatable, userdata, string
	lua_pushcclosure(m_lua_state, &IndexCallee, 2);//Stack: metatable, metatable, CFunction
	lua_setfield(m_lua_state, -2, "__index");//Stack: metatable, metatable
	lua_setmetatable(m_lua_state, -2);//Stack: metatable
	lua_pop(m_lua_state, 1);//Stack:
}

void* CScriptHandlerLua::GetUserData(lua_State *L, int index)
{
	if(!lua_istable(L, index)) return nullptr;
	lua_pushstring(L, "__self");
	lua_rawget(L, index);
	if (lua_islightuserdata(L, -1))
	{
		void* result = lua_touserdata(L, -1);
		lua_pop(L, 1);
		return result;
	}
	
	lua_pop(L, 1);
	return nullptr;
}

int CScriptHandlerLua::GetClassAndInstance(lua_State *L, void ** instance, sLuaClass ** classPtr)
{
	auto ptr = static_cast<CScriptHandlerLua*>(lua_touserdata(L, lua_upvalueindex(1)));
	if (!ptr)
	{
		return luaL_error(L, "Cannot get script handler instance. Stack is probably corrupted.");
	}
	std::string className = lua_tostring(L, lua_upvalueindex(2));
	auto cl = ptr->m_classes.find(className);
	if (cl == ptr->m_classes.end())
	{
		return luaL_error(L, ("Class " + className + " is not registered").c_str());
	}
	*classPtr = &cl->second;
	*instance = GetUserData(L, 1);
	return 0;
}

int CScriptHandlerLua::NewClassInstance(lua_State *L, void* ptr, std::string const& className)
{
	if (!ptr)
	{
		lua_pushnil(L);//Stack:nil
		return 1;
	}
	luaL_checktype(L, 1, LUA_TTABLE);//Stack:
	lua_newtable(L);//Stack: table
	lua_pushstring(L, "__index");//stack: table, string
	lua_pushvalue(L, 1); //Stack: table, string, table
	lua_rawset(L, 1);  //Stack: table
	lua_pushstring(L, "__self");//Stack: table, string
	lua_pushlightuserdata(L, ptr);//Stack: table, string, userdata
	luaL_getmetatable(L, ("Classes." + className).c_str());//Stack: table, string, userdata, metatable
	lua_setmetatable(L, -4);//Stack: table, string, userdata
	lua_rawset(L, -3);//Stack: table
	return 1;
}