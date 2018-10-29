// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "T1AnimInstance.generated.h"

/**
 * Ŀ���� ����¡�� �ִϸ��̼� �ν��Ͻ� ó����
 */
UCLASS()
class T1PROJECT_API UT1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UT1AnimInstance();
	virtual void NativeUpdateAnimation(float deltaSecond) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;
	
};
