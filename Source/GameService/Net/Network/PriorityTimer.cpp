
#include "PriorityTimer.h"

#include <chrono>


namespace
{
	int64_t GetNowTick()
	{
		static auto start = std::chrono::steady_clock::now();

		auto duration = std::chrono::steady_clock::now() - start;
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() + 1;
	}
}

void PriorityTimer::push(std::shared_ptr<Task> task, int64_t timeoutMs)
{
	int64_t expireTick = GetNowTick() + timeoutMs;

	std::lock_guard<std::mutex> guard(lock_);
	timer_.emplace(task, expireTick);
}

std::vector<std::shared_ptr<Task>> PriorityTimer::getExpiredTask()
{
	std::vector<std::shared_ptr<Task>> tasks;
	auto nowTick = GetNowTick();

	{
		std::lock_guard<std::mutex> guard(lock_);
		while (timer_.empty() == false)
		{
			auto& node = timer_.top();
			if (node.expireTick > nowTick)
			{
				break;
			}

			tasks.emplace_back(node.task);
			timer_.pop();
		}
	}

	return tasks;
}
