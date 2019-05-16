
#include "Task.h"


Task::~Task()
{
}

void Task::process(TaskExecuter* executer)
{
	if (onExecute(executer) == true)
	{
		onEnd(executer);
	}
}
