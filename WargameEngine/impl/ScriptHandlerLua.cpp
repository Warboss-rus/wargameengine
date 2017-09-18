#include "ScriptHandlerLua.h"
#include "../LogWriter.h"
#include "../Utils.h"
#include <lua.hpp>

using namespace wargameEngine;

class CLuaRegistryValue
{
public:
	CLuaRegistryValue(lua_State* lua_state)
		: m_lua_state(lua_state)
	{
		m_index = luaL_ref(m_lua_state, LUA_REGISTRYINDEX);
	}
	~CLuaRegistryValue()
	{
		luaL_unref(m_lua_state, LUA_REGISTRYINDEX, m_index);
	}
	void GetValue()
	{
		lua_rawgeti(m_lua_state, LUA_REGISTRYINDEX, m_index);
	}

private:
	lua_State* m_lua_state;
	int m_index;
};

class CLuaArguments : public IArguments
{
public:
	CLuaArguments(lua_State* lua_state, int diff = 0)
		: m_lua_state(lua_state)
		, m_diff(diff)
	{
	}

	virtual int GetCount() const override
	{
		return lua_gettop(m_lua_state) - m_diff;
	}

	virtual bool GetBool(int index) const override
	{
		return lua_toboolean(m_lua_state, index + m_diff) != 0;
	}

	virtual std::string GetStr(int index) const override
	{
		return luaL_checkstring(m_lua_state, index + m_diff);
	}

	virtual std::wstring GetWStr(int index) const override
	{
		return Utf8ToWstring(GetStr(index));
	}

	virtual Path GetPath(int index) const override
	{
		return GetStr(index);
	}

	virtual int GetInt(int index) const override
	{
		return static_cast<int>(luaL_checkinteger(m_lua_state, index + m_diff));
	}

	virtual size_t GetSizeT(int index) const override
	{
		return static_cast<size_t>(luaL_checkinteger(m_lua_state, index + m_diff));
	}

	virtual double GetDbl(int index) const override
	{
		return static_cast<double>(luaL_checknumber(m_lua_state, index + m_diff));
	}

	virtual float GetFloat(int index) const override
	{
		return static_cast<float>(luaL_checknumber(m_lua_state, index + m_diff));
	}

	virtual long GetLong(int index) const override
	{
		return static_cast<long>(luaL_checkinteger(m_lua_state, index + m_diff));
	}

	virtual void* GetClassInstance(int index) const override
	{
		return CScriptHandlerLua::GetUserData(m_lua_state, index + m_diff);
	}

	virtual std::function<void(const FunctionArguments& arguments)> GetFunction(int index) const override
	{
		lua_State* lua_state = m_lua_state;
		if (lua_isfunction(m_lua_state, index + m_diff))
		{
			lua_pushvalue(m_lua_state, index + m_diff);
			std::shared_ptr<CLuaRegistryValue> storedValue = std::make_shared<CLuaRegistryValue>(lua_state);
			return [lua_state, storedValue](const FunctionArguments& arguments) {
				storedValue->GetValue();
				CScriptHandlerLua::CallFunctionImpl(arguments, lua_state);
			};
		}
		else
		{
			std::string funcName = GetStr(index);
			return [lua_state, funcName](const FunctionArguments& arguments) {
				lua_getglobal(lua_state, funcName.c_str());
				CScriptHandlerLua::CallFunctionImpl(arguments, lua_state);
			};
		}
	}

	virtual std::vector<int> GetIntArray(int index) const override
	{
		luaL_checktype(m_lua_state, index + m_diff, LUA_TTABLE);
		int n = 0;
		std::vector<int> result;
		for (;;)
		{
			lua_rawgeti(m_lua_state, index + m_diff, ++n);
			if (lua_isnil(m_lua_state, -1))
				break;
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
			if (lua_isnil(m_lua_state, -1))
				break;
			result.push_back(GetFloat(-1 - m_diff));
			lua_pop(m_lua_state, 1);
		}
		lua_pop(m_lua_state, 1);
		return result;
	}

	virtual std::vector<std::wstring> GetStrArray(int index) const override
	{
		luaL_checktype(m_lua_state, index + m_diff, LUA_TTABLE);
		int n = 0;
		std::vector<std::wstring> result;
		for (;;)
		{
			lua_rawgeti(m_lua_state, index + m_diff, ++n);
			if (lua_isnil(m_lua_state, -1))
				break;
			result.push_back(GetWStr(-1));
			lua_pop(m_lua_state, 1);
		}
		lua_pop(m_lua_state, 1);
		return result;
	}

	virtual bool IsNil(int index) const override
	{
		return lua_isnil(m_lua_state, index + m_diff);
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

int CScriptHandlerLua::luaError(lua_State* L)
{
	std::string str = lua_tostring(L, -1);
	LogWriter::WriteLine("LUA error:" + str);
	lua_pop(L, 1);
	throw std::runtime_error(str);
}

template<class T>
void PushLuaArray(lua_State* L, std::vector<T> const& arr, std::function<void(T const& value)> const& func)
{
	lua_createtable(L, static_cast<int>(arr.size()), 0);
	for (size_t i = 0; i < arr.size(); ++i)
	{
		func(arr[i]);
		lua_rawseti(L, -2, i + 1);
	}
}

class ReturnValueVisitor
{
public:
	lua_State* L;

	int operator()(std::monostate)
	{
		lua_pushnil(L);
		return 0;
	}

	int operator()(std::nullptr_t)
	{
		lua_pushnil(L);
		return 0;
	}

	int operator()(bool value)
	{
		lua_pushboolean(L, value);
		return 1;
	}

	int operator()(long long value)
	{
		lua_pushinteger(L, value);
		return 1;
	}

	int operator()(double value)
	{
		lua_pushnumber(L, value);
		return 1;
	}

	int operator()(const std::string& value)
	{
		lua_pushstring(L, value.c_str());
		return 1;
	}

	int operator()(const std::wstring& value)
	{
		lua_pushstring(L, WStringToUtf8(value).c_str());
		return 1;
	}

	int operator()(const FunctionArgument::ClassInstance& instance)
	{
		return CScriptHandlerLua::NewClassInstance(L, instance.ptr, instance.className);
	}

	int operator()(const std::vector<FunctionArgument>& value)
	{
		PushLuaArray<FunctionArgument>(L, value, [this](FunctionArgument const& arg) { std::visit(*this, arg.GetVariant()); });
		return 1;
	}

	int operator()(const std::map<std::string, FunctionArgument>& value)
	{
		lua_createtable(L, static_cast<int>(value.size()), 0);
		for (auto& pair : value)
		{
			lua_pushstring(L, pair.first.c_str());
			std::visit(*this, pair.second.GetVariant());
			lua_rawset(L, -3);
		}
		return 1;
	}
};

int CScriptHandlerLua::PushReturnValue(lua_State* L, FunctionArgument const& arg)
{
	ReturnValueVisitor visitor{ L };
	return std::visit(visitor, arg.GetVariant());
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

void CScriptHandlerLua::Reset()
{
	lua_close(m_lua_state);
	m_functions.clear();
	m_classes.clear();
	m_lua_state = luaL_newstate();
	lua_register(m_lua_state, "_ALERT", luaError);
	lua_atpanic(m_lua_state, luaError);
	luaL_openlibs(m_lua_state);
}

void CScriptHandlerLua::RunScript(const Path& path)
{
	int result = luaL_dofile(m_lua_state, path.string().c_str());
	if (result && lua_isstring(m_lua_state, -1))
	{
		const char* err = lua_tostring(m_lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

void CScriptHandlerLua::CallFunctionImpl(FunctionArguments const& arguments, lua_State* lua_state)
{
	for (auto& arg : arguments)
	{
		PushReturnValue(lua_state, arg);
	}
	int result = lua_pcall(lua_state, static_cast<int>(arguments.size()), 0, 0);
	if (result && lua_isstring(lua_state, -1))
	{
		const char* err = lua_tostring(lua_state, -1);
		LogWriter::WriteLine(std::string("LUA Error: ") + err);
	}
}

void CScriptHandlerLua::CallFunction(const std::string& funcName, const FunctionArguments& arguments)
{
	lua_getglobal(m_lua_state, funcName.c_str());
	CallFunctionImpl(arguments, m_lua_state);
}

void CScriptHandlerLua::RegisterConstant(const std::string& name, FunctionArgument const& value)
{
	PushReturnValue(m_lua_state, value);
	lua_setglobal(m_lua_state, name.c_str());
}

void CScriptHandlerLua::RegisterFunction(const std::string& name, FunctionHandler const& handler)
{
	auto ptr = m_functions.emplace(name, std::make_unique<FunctionHandler>(handler)).first->second.get();
	lua_pushlightuserdata(m_lua_state, ptr); //Stack: userdata
	lua_pushcclosure(m_lua_state, &FunctionCallee, 1); //Stack: CFunction
	lua_setglobal(m_lua_state, name.c_str()); //Stack:
}

void CScriptHandlerLua::RegisterMethod(const std::string& className, const std::string& methodName, MethodHandler const& handler)
{
	std::string classNameStr = className;
	if (m_classes.find(classNameStr) == m_classes.end())
	{
		RegisterClass(classNameStr);
	}
	auto& cl = m_classes[classNameStr];
	auto ptr = cl.methods.emplace(methodName, std::make_unique<MethodHandler>(handler)).first->second.get();

	lua_getglobal(m_lua_state, classNameStr.c_str()); //Stack: metatable
	int index = lua_gettop(m_lua_state);
	lua_pushstring(m_lua_state, methodName.c_str()); //Stack: metatable, string
	lua_pushlightuserdata(m_lua_state, ptr); //Stack: metatable, string, userdata
	lua_pushcclosure(m_lua_state, &MethodCallee, 1); //Stack: metatable, string, CFunction
	lua_rawset(m_lua_state, index); //Stack: metatable
	lua_pop(m_lua_state, 1);
}

void CScriptHandlerLua::RegisterProperty(const std::string& className, const std::string& propertyName, SetterHandler const& setterHandler, GetterHandler const& getterHandler)
{
	if (m_classes.find(className) == m_classes.end())
	{
		RegisterClass(className);
	}
	auto& cl = m_classes[className];
	cl.getters.emplace(propertyName, getterHandler);
	cl.setters.emplace(propertyName, setterHandler);
}

void CScriptHandlerLua::RegisterProperty(const std::string& className, const std::string& propertyName, GetterHandler const& getterHandler)
{
	if (m_classes.find(className) == m_classes.end())
	{
		RegisterClass(className);
	}
	auto& cl = m_classes[className];
	cl.getters.emplace(propertyName, getterHandler);
}

int CScriptHandlerLua::FunctionCallee(lua_State* L)
{
	auto ptr = static_cast<FunctionHandler*>(lua_touserdata(L, lua_upvalueindex(1)));
	if (!ptr)
	{
		return luaL_error(L, "Cannot get function pointer. Stack is probably corrupted.");
	}
	CLuaArguments args(L);
	try
	{
		return PushReturnValue(L, (*ptr)(args));
	}
	catch (std::exception const& e)
	{
		return luaL_error(L, e.what());
	}
}

int CScriptHandlerLua::MethodCallee(lua_State* L)
{
	void* instance = GetUserData(L, 1);
	auto ptr = static_cast<MethodHandler*>(lua_touserdata(L, lua_upvalueindex(1)));
	if (!ptr)
	{
		return luaL_error(L, "Cannot get method pointer. Stack is probably corrupted.");
	}
	try
	{
		CLuaArguments args(L, 1);
		return PushReturnValue(L, (*ptr)(instance, args));
	}
	catch (std::exception const& e)
	{
		return luaL_error(L, e.what());
	}
}

int CScriptHandlerLua::IndexCallee(lua_State* L)
{
	void* instance;
	sLuaClass* cl;
	int result = GetClassAndInstance(L, &instance, &cl);
	if (result)
		return result;

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
	if (result)
		return result;

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
	luaL_newmetatable(m_lua_state, ("Classes." + className).c_str()); //Stack: metatable
	lua_pushlightuserdata(m_lua_state, this); //Stack: metatable, userdata
	lua_pushstring(m_lua_state, className.c_str()); //Stack: metatable, userdata, string
	lua_pushcclosure(m_lua_state, &NewIndexCallee, 2); //Stack: metatable, CFunction
	lua_setfield(m_lua_state, -2, "__newindex"); //Stack: metatable
	lua_pushvalue(m_lua_state, -1); //Stack: metatable, metatable
	lua_setglobal(m_lua_state, className.c_str()); //Stack: metatable

	//this calls IndexCallee, but the table has no __self field
	luaL_newmetatable(m_lua_state, ("ClassesIndex." + className).c_str()); //Stack: metatable, metatable
	lua_pushlightuserdata(m_lua_state, this); //Stack: metatable, metatable, userdata
	lua_pushstring(m_lua_state, className.c_str()); //Stack: metatable, metatable, userdata, string
	lua_pushcclosure(m_lua_state, &IndexCallee, 2); //Stack: metatable, metatable, CFunction
	lua_setfield(m_lua_state, -2, "__index"); //Stack: metatable, metatable
	lua_setmetatable(m_lua_state, -2); //Stack: metatable
	lua_pop(m_lua_state, 1); //Stack:
}

void* CScriptHandlerLua::GetUserData(lua_State* L, int index)
{
	if (!lua_istable(L, index))
		return nullptr;
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

int CScriptHandlerLua::GetClassAndInstance(lua_State* L, void** instance, sLuaClass** classPtr)
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

int CScriptHandlerLua::NewClassInstance(lua_State* L, void* ptr, std::string const& className)
{
	if (!ptr)
	{
		lua_pushnil(L); //Stack:nil
		return 1;
	}
	luaL_checktype(L, 1, LUA_TTABLE); //Stack:
	lua_newtable(L); //Stack: table
	lua_pushstring(L, "__index"); //stack: table, string
	lua_pushvalue(L, 1); //Stack: table, string, table
	lua_rawset(L, 1); //Stack: table
	lua_pushstring(L, "__self"); //Stack: table, string
	lua_pushlightuserdata(L, ptr); //Stack: table, string, userdata
	luaL_getmetatable(L, ("Classes." + className).c_str()); //Stack: table, string, userdata, metatable
	lua_setmetatable(L, -4); //Stack: table, string, userdata
	lua_rawset(L, -3); //Stack: table
	return 1;
}