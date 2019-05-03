#pragma once

class GSFStageMode
{
public:
	enum Lobby
	{
		INTRO,
		ASSET_DOWNLOAD,
		SERVER_SELECT,
		CAHRACTOR_SELECT,
		CHARACTOR_CREATE,
		CHARACTOR_PREVIEW,
		LOBBY_MAX,
	};

	enum Game
	{
		TOWN,
		FIELD,
		DUNGEON,
		GAME_MAX,
	};
};