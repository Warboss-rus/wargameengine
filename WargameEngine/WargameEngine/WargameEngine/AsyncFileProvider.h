#pragma once
#include <string>
#include <functional>

class ThreadPool;
struct sModule;

class CAsyncFileProvider
{
public:
	CAsyncFileProvider(ThreadPool & threadPool, std::string const& workingDir);
	void SetModule(sModule const& module);
	typedef std::function<void(void*, unsigned int)> ProcessHandler;
	typedef std::function<void()> CompletionHandler;
	typedef std::function<void(std::exception const&)> ErrorHandler;
	void GetTextureAsync(std::string const& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler = ErrorHandler(), bool now = false);
	void GetModelAsync(std::string const& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler = ErrorHandler());
	std::string GetModelAbsolutePath(std::string const& path) const;
	std::string GetScriptAbsolutePath(std::string const& path) const;
	std::string GetShaderAbsolutePath(std::string const& path) const;
	std::string GetAbsolutePath(std::string const& path) const;
private:
	ThreadPool & m_threadPool;
	std::string m_workingDir;
	std::string m_moduleDir;
	std::string m_textureDir;
	std::string m_modelDir;
	std::string m_scriptDir;
	std::string m_shaderDir;
};

std::string AppendPath(std::string const& oldPath, std::string const& newPath);