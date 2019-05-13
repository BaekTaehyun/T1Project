#pragma once

class FGsGameMode
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

