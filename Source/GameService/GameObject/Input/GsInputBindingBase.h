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
 * 추후 바인딩 관련 구조체를 설정하게 만들어 바인딩 시키는 구조로 수정이 필요해 보임
 * UGameObject 스폰 -> Actor에 InputBinder 설정 -> Actor의 SetupPlayerInputComponent() 호출로 SetBinding() 실행
 */
UCLASS()
class GAMESERVICE_API UGsInputBindingBase : public UObject
{
	GENERATED_BODY()
	
public:
	UGsInputBindingBase();
	virtual ~UGsInputBindingBase();

	virtual void Initialize();
	virtual void Finalize();

	virtual void SetBinding(UInputComponent* input);

protected:
	UInputComponent* InputComponent;
};