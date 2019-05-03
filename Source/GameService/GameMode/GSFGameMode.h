#pragma once

class GSFGameMode
{
public:
	enum Mode
	{		
		LOBBY,
		GAME,
		MAX,
	};

	enum ModeMessage
	{
		ENTER_MODE,
		LEAVE_MODE,
	};
};

