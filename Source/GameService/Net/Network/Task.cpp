
#include "Task.h"


Task::~Task()
{
}

void Task::process(TaskExecuter* executer)
{
	try
	{
		if (onExecute(executer) == true)
		{
			onEnd(executer);
		}
	}
	catch (...)
	{
		result_ = false;
		onEnd(executer);
	}
}
