#pragma once
class MessageLobby
{
public:
	enum Hive
	{
		GVS_DOWNLOAD_COMPLETE,
		HIVE_LOGIN_COMPLETE,
	};
	enum Stage
	{
		INTRO_COMPLETE,
		ASSETDOWN_COMPLETE,
		LOGIN_COMPLETE,
		SERVER_SELECTCOMPLETE,
	};
};