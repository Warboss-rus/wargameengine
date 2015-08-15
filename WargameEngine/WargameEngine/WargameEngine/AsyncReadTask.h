#pragma once
#include "Task.h"

class AsyncReadTask : public TaskBase<void>
{
public:
	typedef std::function<void(void*, unsigned int)> AsyncReadHandler;
	AsyncReadTask(std::string const& file, AsyncReadHandler const& handler)
		:m_handler(handler)
		, m_path(file)
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
				throw std::exception(("Cannot open file " + m_path).c_str());
			}
			fseek(file, 0L, SEEK_END);
			unsigned int size = ftell(file);
			fseek(file, 0L, SEEK_SET);
			m_data.resize(size);
			fread(m_data.data(), 1, size, file);
			fclose(file);
			if (m_handler)
			{
				ThreadPool::RunFunc([this]() {
					try
					{
						m_handler(m_data.data(), m_data.size());
					}
					catch (std::exception const& e)
					{
						SetTaskState(TaskState::FAILED);
						if (m_onFail)
						{
							ThreadPool::QueueCallback([=]() {m_onFail(e);});
						}
					}
				}, [this]() {
					m_callback();
					SetTaskState(ITask::TaskState::COMPLETED);
					ThreadPool::RemoveTask(this);
				});
			}
		}
		catch (std::exception const& e)
		{
			SetTaskState(TaskState::FAILED);
			if (m_onFail)
			{
				ThreadPool::QueueCallback([=]() {m_onFail(e);});
			}
			ThreadPool::RemoveTask(this);
		}
	}
private:
	std::string m_path;
	std::vector<unsigned char> m_data;
	AsyncReadHandler m_handler;
};