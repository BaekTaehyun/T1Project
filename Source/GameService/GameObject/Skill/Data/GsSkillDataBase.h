#pragma once

#include "CoreMinimal.h"
#include "GsSkillDataBase.generated.h"

/**
 * 스킬 액션 담당 구조체
 */
USTRUCT(BlueprintType)
struct GAMESERVICE_API FGsSkillActionDataBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int Type;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float Rate;	//실행 타이밍 정보
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int Count;	//수집 대상 갯수(Pram 변수들)
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int RefID;	//효과 참조 정보(Pram 변수들)
};

/**
* 스킬 정보 구조체
*/
USTRUCT(BlueprintType)
struct GAMESERVICE_API FGsSkillDataBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKill")
	int ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKill")
	float Duration;
	//추후 Editor상에서 리소스를 드래그하면 Path정보가 입력되는 형태로 구현이 필요함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKill")
	UAnimMontage* ResAni;
	//FString ResAni;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKill")
	TArray<FGsSkillActionDataBase> SkillAction;
};
