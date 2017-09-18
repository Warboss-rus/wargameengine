#pragma once
#include "Typedefs.h"
#include <functional>

namespace wargameEngine
{
class ThreadPool;
struct Module;

class AsyncFileProvider
{
public:
	typedef std::function<void(void*, size_t)> ProcessHandler;
	typedef std::function<void()> CompletionHandler;
	typedef std::function<void(std::exception const&)> ErrorHandler;

	AsyncFileProvider(ThreadPool& threadPool);
	void SetModule(Module const& module);
	void GetTextureAsync(const Path& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler = ErrorHandler(), bool now = false);
	void GetModelAsync(const Path& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler = ErrorHandler());
	Path GetModelAbsolutePath(const Path& path) const;
	Path GetTextureAbsolutePath(const Path& path) const;
	Path GetScriptAbsolutePath(const Path& path) const;
	Path GetShaderAbsolutePath(const Path& path) const;
	Path GetAbsolutePath(const Path& path) const;

private:
	ThreadPool& m_threadPool;
	Path m_workingDir;
	Path m_moduleDir;
	Path m_textureDir;
	Path m_modelDir;
	Path m_scriptDir;
	Path m_shaderDir;
};

std::vector<Path> GetFiles(const Path& path, const Path& mask, bool recursive);
}