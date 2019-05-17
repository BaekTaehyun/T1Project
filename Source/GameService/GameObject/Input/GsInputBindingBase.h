// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "GsInputBindingBase.generated.h"

class UInputComponent;

struct FGsInputParams
{
	FName Name;
	EInputEvent KeyEvent;
	FInputTouchUnifiedDelegate del1;
	FInputAxisUnifiedDelegate del2;
	FInputVectorAxisUnifiedDelegate del3;
	FInputGestureUnifiedDelegate del4;
};

/**
 * 키입력 관련 바인딩 클래스
 * [Todo]
 * 현재는 단순히 클래스만 따로둔 형태이지만
 * 추후 바인딩 관련 구조체를 설정해게 만들어 바인딩 시키는 구조로 수정이 필요해 보임
 */
UCLASS()
class GAMESERVICE_API UGsInputBindingBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Initialize();
	virtual void Finalize();

	virtual void SetBinding(UInputComponent* input);

protected:
	//TArray<FInputParams> Params;
	UInputComponent* InputComponent;
};