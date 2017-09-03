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

void AsyncFileProvider::SetModule(Module const& module)
{
	m_moduleDir = module.folder;
	m_textureDir = m_moduleDir / module.textures;
	m_modelDir = m_moduleDir / module.models;
	m_scriptDir = m_moduleDir;
	m_shaderDir = m_moduleDir / module.shaders;
}

void AsyncFileProvider::GetTextureAsync(const Path& path, ProcessHandler const& processHandler, CompletionHandler const& completionHandler, ErrorHandler const& errorHandler, bool now)
{
	std::shared_ptr<AsyncReadTask> readTask = std::make_shared<AsyncReadTask>(m_textureDir / path, processHandler, m_threadPool);
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
	std::shared_ptr<AsyncReadTask> readTask = std::make_shared<AsyncReadTask>(m_modelDir / path, processHandler, m_threadPool);
	readTask->AddOnCompleteHandler(completionHandler);
	readTask->AddOnFailHandler(errorHandler);
	m_threadPool.AddTask(readTask);
}

Path AsyncFileProvider::GetModelAbsolutePath(const Path& path) const
{
	return m_modelDir / path;
}

Path AsyncFileProvider::GetTextureAbsolutePath(const Path& path) const
{
	return m_textureDir / path;
}

Path AsyncFileProvider::GetScriptAbsolutePath(const Path& path) const
{
	return m_scriptDir / path;
}

Path AsyncFileProvider::GetShaderAbsolutePath(const Path& path) const
{
	return m_shaderDir / path;
}

Path AsyncFileProvider::GetAbsolutePath(const Path& path) const
{
	return m_moduleDir / path;
}

bool match(char const* needle, char const* haystack)
{
	for (; *needle != '\0'; ++needle)
	{
		switch (*needle)
		{
		case '?':
			++haystack;
			break;
		case '*':
		{
			size_t max = strlen(haystack);
			if (needle[1] == '\0' || max == 0)
				return true;
			for (size_t i = 0; i < max; i++)
				if (match(needle + 1, haystack + i))
					return true;
			return false;
		}
		default:
			if (*haystack != *needle)
				return false;
			++haystack;
		}
	}
	return *haystack == '\0';
}

namespace fs = std::experimental::filesystem;

std::vector<Path> GetFiles(const Path& path, const Path& mask, bool recursive)
{
	std::vector<Path> result;
	Path absolutePath = fs::absolute(path);
	auto process = [&result, &mask, &absolutePath](auto it) {
		for (const fs::directory_entry& p : it)
		{
			auto& path = p.path();
			if (fs::is_regular_file(path) && match(mask.string().c_str(), path.string().c_str()))
			{
				result.push_back(path.filename());
			}
		}
	};
	if(recursive)
	{
		process(fs::recursive_directory_iterator(absolutePath));
	}
	else
	{
		process(fs::directory_iterator(absolutePath));
	}
	std::sort(result.begin(), result.end());
	return result;
}
}