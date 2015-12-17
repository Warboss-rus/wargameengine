#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>

class IArguments
{
public:
	virtual ~IArguments() {}

	virtual int GetCount() const = 0;

	virtual bool GetBool(int index) const = 0;
	virtual std::string GetStr(int index) const = 0;
	virtual int GetInt(int index) const = 0;
	virtual double GetDbl(int index) const = 0;
	virtual float GetFloat(int index) const = 0;
	virtual void* GetClassInstance(int index) const = 0;
	virtual std::vector<int> GetIntArray(int index) const = 0;
	virtual std::vector<float> GetFloatArray(int index) const = 0;
	virtual std::vector<std::string> GetStrArray(int index) const = 0;

	virtual bool IsBool(int index) const = 0;
	virtual bool IsStr(int index) const = 0;
	virtual bool IsNumber(int index) const = 0;
	virtual bool IsClassInstance(int index) const = 0;
	virtual bool IsIntArray(int index) const = 0;
	virtual bool IsFloatArray(int index) const = 0;
	virtual bool IsStrArray(int index) const = 0;
};

struct FunctionArgument
{
	FunctionArgument()
		:type(Type::TNULL)
	{}
	FunctionArgument(nullptr_t)
		:type(Type::TNULL)
	{}
	FunctionArgument(bool value)
		: type(Type::BOOLEAN), data(std::make_shared<bool>(value))
	{}
	FunctionArgument(int value)
		: type(Type::INT), data(std::make_shared<int>(value))
	{}
	FunctionArgument(float value)
		: type(Type::FLOAT), data(std::make_shared<float>(value))
	{}
	FunctionArgument(double value)
		: type(Type::DOUBLE), data(std::make_shared<double>(value))
	{}
	FunctionArgument(std::string value)
		: type(Type::STRING), data(std::make_shared<std::string>(value))
	{}
	FunctionArgument(std::vector<int> value)
		: type(Type::INT_ARRAY), data(std::make_shared<std::vector<int>>(value))
	{}
	FunctionArgument(std::vector<float> value)
		: type(Type::FLOAT_ARRAY), data(std::make_shared<std::vector<float>>(value))
	{}
	FunctionArgument(std::vector<std::string> value)
		: type(Type::STRING_ARRAY), data(std::make_shared<std::vector<std::string>>(value))
	{}
	FunctionArgument(void* ptr, std::string const& className)
		:type(Type::CLASS_INSTANCE), data(std::make_shared<sClassInstance>(ptr, className))
	{}

	enum class Type
	{
		TNULL,
		BOOLEAN,
		INT,
		FLOAT,
		DOUBLE,
		STRING,
		INT_ARRAY,
		FLOAT_ARRAY,
		STRING_ARRAY,
		CLASS_INSTANCE
	} type;
	struct sClassInstance
	{
		sClassInstance(void* ptr, std::string const& className)
			:ptr(ptr), className(className)
		{}
		void* ptr;
		std::string className;
	};
	std::shared_ptr<void> data;
};

class IScriptHandler
{
public:
	virtual ~IScriptHandler() {}

	virtual void RunScript(std::string const& path) = 0;
	virtual void CallFunction(std::string const& funcName, FunctionArgument const& argument) = 0;
	virtual void RegisterConstant(std::string const& name, std::string const& value) = 0;
	typedef std::function<FunctionArgument(IArguments const& args)> FunctionHandler;
	virtual void RegisterFunction(std::string const& name, FunctionHandler const& handler) = 0;
	typedef std::function<FunctionArgument(void* instance, IArguments const& args)> MethodHandler;
	virtual void RegisterMethod(std::string const& className, std::string const& methodName, MethodHandler const& handler) = 0;
	typedef std::function<void(void* instance, IArguments const& args)> SetterHandler;
	typedef std::function<FunctionArgument(void* instance)> GetterHandler;
	virtual void RegisterProperty(std::string const& className, std::string const& propertyName, SetterHandler const& setterHandler, GetterHandler const& getterHandler) = 0;
};
