#pragma once
#include "Task.h"

class AsyncReadTask : public TaskT<void>
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
			m_size = ftell(file);
			fseek(file, 0L, SEEK_SET);
			m_data = new unsigned char[m_size];
			fread(m_data, 1, m_size, file);
			fclose(file);
			if (m_handler)
			{
				ThreadPool::RunFunc([this]() {
					try
					{
						m_handler(m_data, m_size);
					}
					catch (std::exception const& e)
					{
						SetTaskState(TaskState::FAILED);
						if (m_onFail)
						{
							ThreadPool::QueueCallback([=]() {m_onFail(e);});
						}
					}
				}, m_callback);
			}
		}
		catch (std::exception const& e)
		{
			SetTaskState(TaskState::FAILED);
			if (m_onFail)
			{
				ThreadPool::QueueCallback([=]() {m_onFail(e);});
			}
		}
	}
private:
	std::string m_path;
	void * m_data;
	unsigned int m_size;
	AsyncReadHandler m_handler;
};