// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameObject/Component/GsAnimInstanceState.h"
#include "GameService.h"
#include "GsLocalCharacter.generated.h"

/**
* 언리얼 엔진 로직 담당 Local캐릭터 전용 클래스
* 로컬 BP제작시 이 클래스를 기반으로 제작
* ACharacter 클래스를 상속받은 커스텀 ACharacter 클래스
* 조작 제어를 위해 UGsInputBindingBase를 설정하게 구성
*/
UCLASS()
class GAMESERVICE_API AGsLocalCharacter : public ACharacter
{
	GENERATED_BODY()

	//캐릭터 따라가기 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	//InputBinder
	class UGsInputBindingBase* InputBinder;
	//Anim
	UGsAnimInstanceState* Animation;

public:
	FORCEINLINE UGsAnimInstanceState* GetAnim() const				{ return Animation; }
	FORCEINLINE UGsInputBindingBase* GetInputBinder() const			{ return InputBinder; }
	FORCEINLINE USpringArmComponent* GetSpringArm() const			{ return CameraBoom; }

	FORCEINLINE void SetInputBinder(UGsInputBindingBase* Binder)	{ InputBinder = Binder; }

public:
	// Sets default values for this character's properties
	AGsLocalCharacter();
	virtual ~AGsLocalCharacter();
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void EnableCollision();
	void DisableCollision();

protected:
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	ECollisionEnabled::Type RestoreCollisionType;
};
