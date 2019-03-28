// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "T1Project.h"
#include "Components/ActorComponent.h"
#include "T1PlayerStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class T1PROJECT_API UT1PlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UT1PlayerStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

public:
	void SetNewLevel(int32 NewLevel);

private:
	struct FT1PlayerData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	// �𸮾� ������Ʈ���� ����ȭ ����� �־ ������Ʈ�� UPROPERY �Ӽ��� �����ϰ� �ε��Ҽ� �ִ�
	// ������ �ʿ䰡���� �Ӽ����� Transient(�ӽ�) �Ӽ��� �ο��Ͽ� �������� �ʵ��� �Ѵ�.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	float CurrentHP;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
