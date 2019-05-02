#pragma once

// 로비 UI에서 C++쪽으로 타이밍등 이벤트 전달용
UENUM(BluePrintType)
enum class EGS_LOBBY_Enum : uint8
{
	GS_LOBBY_START_INTRO	UMETA(DisplayName = "LOBBY_START_INTRO"),
	GS_LOBBY_END_INTRO		UMETA(DisplayName = "LOBBY_END_INTRO")
};
