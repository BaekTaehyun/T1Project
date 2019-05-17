#pragma once

#include "CoreMinimal.h"
// 서비스 타입 이넘
enum class EGsServiceType : uint8
{
	live,
	submission,
	qa,
	dev,
	Test01,    // 임시 테스트용.
	Test02,    // 임시 테스트용.
	MAX
};