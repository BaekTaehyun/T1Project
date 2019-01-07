// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "T1Project.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "DataActorComponent.h"

#include "T1Player.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UT1AnimInstance;


UCLASS()
class T1PROJECT_API AT1Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AT1Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 카메라모드를 제어
	enum class ECameraControlMode
	{
		FOLLOW,
		DIABLO,
	};
	
	ECameraControlMode CurrentCameraControlMode = ECameraControlMode::FOLLOW;
	FVector DirectionToMove = FVector::ZeroVector;

	// 카메라 보간용
	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;
	float ArmRotationSpeed = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 콜링 순서를 파악 하기 위한 오버라이딩	
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	// 언리얼 내부 데이지 처리예
	virtual float TakeDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = CharInfo)
	class UDataActorComponent* CharInfoData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	
private:
	// 프로젝트 옵션에 인풋과 매칭되는 함수
	void UpDown(float newAxisValue);
	void LeftRight(float newAxisValue);
	void Turn(float newAxisValue);
	void LookUp(float newAxisValue);

	void CameraViewChange();
	void SetCameraControlMode(ECameraControlMode inControlMode);

	void Attack();
	void AttackStartComboState();
	void AttackEndComboState();

	// 피직스 충돌설정을 확인하기위한함수
	void AttackCheck();

	/*
		언리얼에서 델리게이트는 C++객체에서만 사용하는 델리게이트와 C++, 블루프린트 객체가 모두 사용할수 있는 델리게이트로 나눈다.
		블루프린트 오브젝트는 맴버 함수에 대한 정보를 저장하고 로딩하는 직렬화(Serialization) 매커니즘이 들어가 있기떄문에
		일반 C++ 언어가 관리하는 방법으로 맴버함수를 관리 할수 없다. 그래서 블루프린트와 관련ㄷ된 C++함수는 모두 UFUNCTION 매크로를
		사용해야한다. 이렇게 블루프린트 객체와도 연동되는 델리게이트를 언리얼언진에서는 다이나믹 델리게이트(Dynamic Delegate)라고 한다.
	*/
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentComboIndex;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int MaxComboCount;

	UPROPERTY()
	class UT1AnimInstance* T1Anim;
};
