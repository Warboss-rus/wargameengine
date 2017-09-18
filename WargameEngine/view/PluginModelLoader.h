#pragma once
#include "IModelReader.h"
#include "..\IPlugin.h"
#include "3dModel.h"

namespace wargameEngine
{
namespace view
{
class PluginModelLoader : public IModelReader
{
public:

	PluginModelLoader(std::unique_ptr<IPlugin>&& plugin)
		: m_plugin(std::move(plugin))
	{
		m_reader = reinterpret_cast<New>(m_plugin->GetFunction("New"))();
	}

	~PluginModelLoader()
	{
		reinterpret_cast<Delete>(m_plugin->GetFunction("Delete"))(m_reader);
	}

	virtual bool ModelIsSupported(unsigned char* data, size_t size, const Path& filePath) const override
	{
		return m_reader->ModelIsSupported(data, size, filePath);
	}

	virtual std::unique_ptr<C3DModel> LoadModel(unsigned char* data, size_t size, const C3DModel& dummyModel, const Path& filePath) override
	{
		return m_reader->LoadModel(data, size, dummyModel, filePath);
	}
private:
	typedef IModelReader* (*New)();
	typedef void(*Delete)(IModelReader*);

	std::unique_ptr<IPlugin> m_plugin;
	IModelReader * m_reader;
};
}
}