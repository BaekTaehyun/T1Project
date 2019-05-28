
#pragma once

#include <memory>


class TaskExecuter;
class Task : public std::enable_shared_from_this<Task>
{
protected:

	bool result_ = true;

public:

	virtual ~Task();

	bool isSucceed() const { return result_ == true; }

	void process(TaskExecuter* executer);

	template<typename T>
	std::shared_ptr<T> toSharedPtr()
	{
		return std::static_pointer_cast<T>(shared_from_this());
	}

private:

	virtual bool onExecute(TaskExecuter* executer) = 0;
	virtual void onEnd(TaskExecuter* executer) = 0;

};

