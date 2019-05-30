#pragma once
#include "CoreMinimal.h"

// 카메라 컨트럴 타입 이넘

UENUM(BlueprintType)
enum class EGsControlMode :uint8
{
	Fixed,
	Free,
	Quater,
	Wide,
	Action,
};