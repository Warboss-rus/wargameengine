#pragma once
#include <string>
#include <functional>

class ThreadPool;
struct sModule;

class CAsyncFileProvider
{
public:
	CAsyncFileProvider(ThreadPool & threadPool);
	void SetModule(sModule const& module);
	typedef std::function<void(void*, size_t)> ProcessHandler;
	typedef std::function<void()> CompletionHandler;
	typedef std::function<void(std::exception const&)> ErrorHandler;
	void GetTextureAsync(std::wstring const& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler = ErrorHandler(), bool now = false);
	void GetModelAsync(std::wstring const& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler = ErrorHandler());
	std::wstring GetModelAbsolutePath(std::wstring const& path) const;
	std::wstring GetTextureAbsolutePath(std::wstring const& path) const;
	std::wstring GetScriptAbsolutePath(std::wstring const& path) const;
	std::wstring GetShaderAbsolutePath(std::wstring const& path) const;
	std::wstring GetAbsolutePath(std::wstring const& path) const;
private:
	ThreadPool & m_threadPool;
	std::wstring m_workingDir;
	std::wstring m_moduleDir;
	std::wstring m_textureDir;
	std::wstring m_modelDir;
	std::wstring m_scriptDir;
	std::wstring m_shaderDir;
};

std::wstring AppendPath(std::wstring const& oldPath, std::wstring const& newPath);