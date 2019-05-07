#pragma once
class MessageLobby
{
	enum System
	{
		GVS_DOWNLOAD_COMPLETE,
		RECONNECT_START,
		RECONNECT_END,
	};
	enum Hive
	{
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