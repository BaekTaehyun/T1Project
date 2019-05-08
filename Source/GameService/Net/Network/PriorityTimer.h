
#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <mutex>

#include "Task.h"


class PriorityTimer
{
	struct Node
	{
		std::shared_ptr<Task> task;
		int64_t expireTick;

		Node(std::shared_ptr<Task> t, int64_t e)
			: task(t)
			, expireTick(e)
		{
		}
	};

	struct CompareNode
	{
		bool operator()(const Node& left, const Node& right) const
		{
			return (left.expireTick > right.expireTick);
		}
	};

	std::mutex lock_;
	std::priority_queue<Node, std::vector<Node>, CompareNode> timer_;

public:

	void push(std::shared_ptr<Task> task, int64_t timeoutMs);

	std::vector<std::shared_ptr<Task>> getExpiredTask();

};

