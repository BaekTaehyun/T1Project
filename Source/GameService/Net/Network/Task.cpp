
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
	/* https://answers.unrealengine.com/questions/264921/cannot-use-try-with-exceptions-disabled.html
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
	*/
}
