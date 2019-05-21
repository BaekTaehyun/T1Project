#pragma once

#include "CoreMinimal.h"
#include "GsSkillDataBase.generated.h"

/**
 * ��ų �׼� ��� ����ü
 */
USTRUCT(BlueprintType)
struct GAMESERVICE_API FGsSkillActionDataBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int Type;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float Rate;	//���� Ÿ�̹� ����
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int Count;	//���� ��� ����(Pram ������)
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int RefID;	//ȿ�� ���� ����(Pram ������)
};

/**
* ��ų ���� ����ü
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
	//���� Editor�󿡼� ���ҽ��� �巡���ϸ� Path������ �ԷµǴ� ���·� ������ �ʿ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKill")
	UAnimMontage* ResAni;
	//FString ResAni;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKill")
	TArray<FGsSkillActionDataBase> SkillAction;
};
