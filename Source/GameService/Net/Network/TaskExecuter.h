
#pragma once

#include <memory>
#include <atomic>
#include <vector>
#include <mutex>
#include <thread>

#include "Task.h"
#include "PriorityTimer.h"


class TaskExecuter
{
	std::atomic_bool running_ = false;
	std::thread executer_;

	std::mutex taskLock_;
	std::vector<std::shared_ptr<Task>> tasks_;

	PriorityTimer timer_;

public:

	static TaskExecuter* GetInstance()
	{
		static TaskExecuter executer;
		return &executer;
	}

	void push(std::shared_ptr<Task> task);
	void push(std::shared_ptr<Task> task, int64_t timeoutMs);

private:

	TaskExecuter();
	~TaskExecuter();

	void start();
	void stop();

	void process();

	void doTask();
	void doTimer();

};
