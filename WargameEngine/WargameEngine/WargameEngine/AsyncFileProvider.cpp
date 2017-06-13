#include "AsyncFileProvider.h"
#include "Module.h"
#include "Task.h"
#include "ThreadPool.h"
#include "Utils.h"
#include <vector>

namespace wargameEngine
{

class AsyncReadTask : public TaskBase
{
public:
	typedef std::function<void(void*, size_t)> AsyncReadHandler;
	AsyncReadTask(const Path& file, AsyncReadHandler const& handler, ThreadPool& threadPool)
		: TaskBase(threadPool)
		, m_path(file)
		, m_handler(handler)
	{
	}
	virtual void Execute() override
	{
		std::unique_lock<std::mutex> lk(m_sync);
		if (m_state != TaskState::QUEUED)
		{
			throw std::runtime_error("Task is not ready for execution");
		}
		m_state = TaskState::STARTED;
		lk.unlock();
		try
		{
			m_data = ReadFile(m_path);
			if (m_handler)
			{
				m_threadPool.RunFunc([this]() {
					try
					{
						m_handler(m_data.data(), m_data.size());
					}
					catch (std::exception const& e)
					{
						SetTaskState(TaskState::FAILED);
						if (m_onFail)
						{
							m_threadPool.QueueCallback([=]() {m_onFail(e); });
						}
					} }, [this]() {
					m_callback();
					SetTaskState(ITask::TaskState::COMPLETED);
					m_threadPool.RemoveTask(this); });
			}
		}
		catch (std::exception const& e)
		{
			SetTaskState(TaskState::FAILED);
			if (m_onFail)
			{
				m_threadPool.QueueCallback([=]() {
					m_onFail(e);
					m_threadPool.RemoveTask(this);
				});
			}
		}
	}

private:
	Path m_path;
	std::vector<char> m_data;
	AsyncReadHandler m_handler;
};

AsyncFileProvider::AsyncFileProvider(ThreadPool& threadPool)
	: m_threadPool(threadPool)
{
}

std::vector<Path> SplitPath(const Path& path)
{
	std::vector<Path> pathElements;
	size_t begin = 0;
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (path[i] == '/' || path[i] == '\\')
		{
			pathElements.push_back(path.substr(begin, i - begin));
			begin = i + 1;
		}
	}
	if (begin < path.size())
	{
		pathElements.push_back(path.substr(begin));
	}
	return pathElements;
}

Path MakePath(std::vector<Path> const& parts)
{
	Path result;
	for (auto& part : parts)
	{
		if (!result.empty())
		{
			result += '/';
		}
		result += part;
	}
	return result;
}

bool IsAbsolute(const Path& firstPart)
{
	if (firstPart.empty())
		return true; //Linux absolute path
	if (firstPart.size() == 2 && firstPart[0] >= 'A' && firstPart[0] <= 'Z' && firstPart[1] == ':')
		return true; //Windows drive letter
	return false;
}

Path AppendPath(const Path& oldPath, const Path& newPath)
{
	auto newParts = SplitPath(newPath);
	if (newParts.size() > 0 && IsAbsolute(newParts[0])) //absolute path
	{
		return MakePath(newParts);
	}
	auto existringParts = SplitPath(oldPath);
	for (auto& part : newParts)
	{
		if (part == make_path(L"..") && !existringParts.empty())
		{
			existringParts.pop_back();
		}
		else
		{
			existringParts.push_back(part);
		}
	}
	return MakePath(existringParts);
}

void AsyncFileProvider::SetModule(Module const& module)
{
	m_moduleDir = module.folder;
	m_textureDir = AppendPath(m_moduleDir, module.textures);
	m_modelDir = AppendPath(m_moduleDir, module.models);
	m_scriptDir = m_moduleDir;
	m_shaderDir = AppendPath(m_moduleDir, module.shaders);
}

void AsyncFileProvider::GetTextureAsync(const Path& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler, bool now)
{
	std::shared_ptr<AsyncReadTask> readTask = std::make_shared<AsyncReadTask>(AppendPath(m_textureDir, path), processHandler, m_threadPool);
	readTask->AddOnCompleteHandler(completionHandler);
	readTask->AddOnFailHandler(errorHandler);
	m_threadPool.AddTask(readTask);
	if (now)
	{
		m_threadPool.WaitForTask(*readTask);
	}
}

void AsyncFileProvider::GetModelAsync(const Path& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler /*= ErrorHandler()*/)
{
	std::shared_ptr<AsyncReadTask> readTask = std::make_shared<AsyncReadTask>(AppendPath(m_modelDir, path), processHandler, m_threadPool);
	readTask->AddOnCompleteHandler(completionHandler);
	readTask->AddOnFailHandler(errorHandler);
	m_threadPool.AddTask(readTask);
}

Path AsyncFileProvider::GetModelAbsolutePath(const Path& path) const
{
	return AppendPath(m_modelDir, path);
}

Path AsyncFileProvider::GetTextureAbsolutePath(const Path& path) const
{
	return AppendPath(m_textureDir, path);
}

Path AsyncFileProvider::GetScriptAbsolutePath(const Path& path) const
{
	return AppendPath(m_scriptDir, path);
}

Path AsyncFileProvider::GetShaderAbsolutePath(const Path& path) const
{
	return AppendPath(m_shaderDir, path);
}

Path AsyncFileProvider::GetAbsolutePath(const Path& path) const
{
	return AppendPath(m_moduleDir, path);
}
}