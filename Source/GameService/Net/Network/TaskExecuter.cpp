
#include "TaskExecuter.h"

#include <cstdlib>
#include <chrono>
#include <random>


TaskExecuter::TaskExecuter()
{
	start();
}

TaskExecuter::~TaskExecuter()
{
	stop();
}

void TaskExecuter::start()
{
	stop();

	running_ = true;
	executer_ = std::thread(&TaskExecuter::process, this);
}

void TaskExecuter::stop()
{
	if (running_ == false)
	{
		return;
	}

	running_ = false;
	executer_.join();
	executer_ = std::thread();
}

void TaskExecuter::push(std::shared_ptr<Task> task)
{
	std::lock_guard<std::mutex> guard(taskLock_);
	tasks_.push_back(task);
}

void TaskExecuter::push(std::shared_ptr<Task> task, int64_t timeoutMs)
{
	timer_.push(task, timeoutMs);
}

void TaskExecuter::process()
{
	std::srand(std::random_device()());

	while (running_ == true)
	{
		doTimer();
		doTask();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void TaskExecuter::doTask()
{
	std::vector<std::shared_ptr<Task>> tasks;
	{
		std::lock_guard<std::mutex> guard(taskLock_);
		tasks_.swap(tasks);
	}

	for (auto& task : tasks)
	{
		task->process(this);
	}
}

void TaskExecuter::doTimer()
{
	std::vector<std::shared_ptr<Task>> tasks = timer_.getExpiredTask();
	for (auto& task : tasks)
	{
		task->process(this);
	}
}



