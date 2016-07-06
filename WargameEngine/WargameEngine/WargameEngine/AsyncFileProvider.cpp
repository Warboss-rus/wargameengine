#include "AsyncFileProvider.h"
#include <vector>
#include "ThreadPool.h"
#include "Module.h"
#include "Task.h"

class AsyncReadTask : public TaskBase
{
public:
	typedef std::function<void(void*, size_t)> AsyncReadHandler;
	AsyncReadTask(std::string const& file, AsyncReadHandler const& handler, ThreadPool & threadPool)
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
			FILE * file = fopen(m_path.c_str(), "rb");
			if (!file)
			{
				throw std::runtime_error(("Cannot open file " + m_path).c_str());
			}
			fseek(file, 0L, SEEK_END);
			size_t size = ftell(file);
			fseek(file, 0L, SEEK_SET);
			m_data.resize(size);
			fread(m_data.data(), 1, size, file);
			fclose(file);
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
							m_threadPool.QueueCallback([=]() {m_onFail(e);});
						}
					}
				}, [this]() {
					m_callback();
					SetTaskState(ITask::TaskState::COMPLETED);
					m_threadPool.RemoveTask(this);
				});
			}
		}
		catch (std::exception const& e)
		{
			SetTaskState(TaskState::FAILED);
			if (m_onFail)
			{
				m_threadPool.QueueCallback([=]() {m_onFail(e);});
			}
			m_threadPool.RemoveTask(this);
		}
	}
private:
	std::string m_path;
	std::vector<unsigned char> m_data;
	AsyncReadHandler m_handler;
};

CAsyncFileProvider::CAsyncFileProvider(ThreadPool & threadPool, std::string const & workingDir)
	:m_threadPool(threadPool), m_workingDir(workingDir)
{
}

std::vector<std::string> SplitPath(std::string const &path)
{
	std::vector<std::string> pathElements;
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

std::string MakePath(std::vector<std::string> const& parts)
{
	std::string result;
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

bool IsAbsolute(std::string const& firstPart)
{
	if (firstPart.empty()) return true;//Linux absolute path
	if (firstPart.size() == 2 && firstPart[0] >= 'A' && firstPart[0] <= 'Z' && firstPart[1] == ':') return true;//Windows drive letter
	return false;
}

std::string AppendPath(std::string const& oldPath, std::string const& newPath)
{
	auto newParts = SplitPath(newPath);
	if (newParts.size() > 0 && IsAbsolute(newParts[0]))//absolute path
	{
		return MakePath(newParts);
	}
	auto existringParts = SplitPath(oldPath);
	for (auto& part : newParts)
	{
		if (part == ".." && !existringParts.empty())
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

void CAsyncFileProvider::SetModule(sModule const & module)
{
	m_moduleDir = AppendPath(m_workingDir, module.folder);
	m_textureDir = AppendPath(m_moduleDir, module.textures);
	m_modelDir = AppendPath(m_moduleDir, module.models);
	m_scriptDir = m_moduleDir;
	m_shaderDir = AppendPath(m_moduleDir, module.shaders);
}

void CAsyncFileProvider::GetTextureAsync(std::string const& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler, bool now)
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

void CAsyncFileProvider::GetModelAsync(std::string const& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler /*= ErrorHandler()*/)
{
	std::shared_ptr<AsyncReadTask> readTask = std::make_shared<AsyncReadTask>(AppendPath(m_modelDir, path), processHandler, m_threadPool);
	readTask->AddOnCompleteHandler(completionHandler);
	readTask->AddOnFailHandler(errorHandler);
	m_threadPool.AddTask(readTask);
}

std::string CAsyncFileProvider::GetModelAbsolutePath(std::string const& path) const
{
	return AppendPath(m_modelDir, path);
}

std::string CAsyncFileProvider::GetScriptAbsolutePath(std::string const& path) const
{
	return AppendPath(m_scriptDir, path);
}

std::string CAsyncFileProvider::GetShaderAbsolutePath(std::string const& path) const
{
	return AppendPath(m_shaderDir, path);
}

std::string CAsyncFileProvider::GetAbsolutePath(std::string const & path) const
{
	return AppendPath(m_moduleDir, path);
}
