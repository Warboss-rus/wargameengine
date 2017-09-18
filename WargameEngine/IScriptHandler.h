#pragma once
#include "Typedefs.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace wargameEngine
{

class FunctionArgument
{
public:
	struct ClassInstance
	{
		ClassInstance(void* ptr, const std::string& className)
			: ptr(ptr)
			, className(className)
		{
		}
		void* ptr;
		std::string className;
	};
	using Data = std::variant<std::monostate, std::nullptr_t, bool, long long, double, std::string, std::wstring, ClassInstance, std::vector<FunctionArgument>>;

	FunctionArgument() = default;
	FunctionArgument(std::nullptr_t) {}
	FunctionArgument(bool data) : m_data(data) {}
	FunctionArgument(long long data) : m_data(data) {}
	FunctionArgument(int data) : m_data(static_cast<long long>(data)) {}
	FunctionArgument(unsigned long long data) : m_data(static_cast<long long>(data)) {}
	FunctionArgument(unsigned int data) : m_data(static_cast<long long>(data)) {}
	FunctionArgument(float data) : m_data(data) {}
	FunctionArgument(double data) : m_data(data) {}
	FunctionArgument(const std::string& data) : m_data(data) {}
	FunctionArgument(const std::wstring& data) : m_data(data) {}
	FunctionArgument(const Path& data) : m_data(data.native()) {}
	FunctionArgument(void* ptr, const std::string& className) : m_data(ClassInstance(ptr, className)) {}
	FunctionArgument(const std::vector<FunctionArgument>& data) : m_data(data) {}

	const Data& GetVariant() const { return m_data; }
	Data& GetVariant() { return m_data; }

private:
	Data m_data;
};
using FunctionArguments = std::vector<FunctionArgument>;

class IArguments
{
public:
	virtual ~IArguments() {}

	virtual int GetCount() const = 0;

	virtual bool GetBool(int index) const = 0;
	virtual std::string GetStr(int index) const = 0;
	virtual std::wstring GetWStr(int index) const = 0;
	virtual Path GetPath(int index) const = 0;
	virtual int GetInt(int index) const = 0;
	virtual long GetLong(int index) const = 0;
	virtual size_t GetSizeT(int index) const = 0;
	virtual double GetDbl(int index) const = 0;
	virtual float GetFloat(int index) const = 0;
	virtual void* GetClassInstance(int index) const = 0;
	virtual std::function<void(FunctionArguments const& arguments)> GetFunction(int index) const = 0;
	virtual std::vector<int> GetIntArray(int index) const = 0;
	virtual std::vector<float> GetFloatArray(int index) const = 0;
	virtual std::vector<std::wstring> GetStrArray(int index) const = 0;

	virtual bool IsBool(int index) const = 0;
	virtual bool IsStr(int index) const = 0;
	virtual bool IsNumber(int index) const = 0;
	virtual bool IsClassInstance(int index) const = 0;
	virtual bool IsIntArray(int index) const = 0;
	virtual bool IsFloatArray(int index) const = 0;
	virtual bool IsStrArray(int index) const = 0;
	virtual bool IsNil(int index) const = 0;
};

template<class T>
std::vector<FunctionArgument> TransformVector(std::vector<T> const& src)
{
	std::vector<FunctionArgument> result;
	std::transform(src.begin(), src.end(), std::back_inserter(result), [](T const& val) { return val; });
	return result;
}

class IScriptHandler
{
public:
	using FunctionHandler = std::function<FunctionArgument(IArguments const& args)>;
	using MethodHandler = std::function<FunctionArgument(void* instance, IArguments const& args)>;
	using SetterHandler = std::function<void(void* instance, IArguments const& args)>;
	using GetterHandler = std::function<FunctionArgument(void* instance)>;

	virtual ~IScriptHandler() {}

	virtual void Reset() = 0;
	virtual void RunScript(const Path& path) = 0;
	virtual void CallFunction(const std::string& funcName, FunctionArguments const& arguments = FunctionArguments()) = 0;
	virtual void RegisterConstant(const std::string& name, FunctionArgument const& value) = 0;
	virtual void RegisterFunction(const std::string& name, FunctionHandler const& handler) = 0;
	virtual void RegisterMethod(const std::string& className, const std::string& methodName, MethodHandler const& handler) = 0;
	virtual void RegisterProperty(const std::string& className, const std::string& propertyName, SetterHandler const& setterHandler, GetterHandler const& getterHandler) = 0;
	virtual void RegisterProperty(const std::string& className, const std::string& propertyName, GetterHandler const& getterHandler) = 0;
};
}