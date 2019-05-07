#pragma once

class GSFStageMode
{
public:
	enum Lobby
	{
		INTRO,
		ASSET_DOWNLOAD,
		SERVER_SELECT,
		CAHRACTER_SELECT,
		CHARACTER_CREATE,
		CHARACTER_PREVIEW,
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