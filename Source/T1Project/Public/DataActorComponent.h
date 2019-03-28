// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "DataActorComponent.generated.h"

USTRUCT(BlueprintType, meta = (ScriptName = "TestUserData", DisplayName = "Test User Data"))
struct FCharInfoData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharInfoTable")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharInfoTable")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharInfoTable")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharInfoTable")
	float MaxMP;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class T1PROJECT_API UDataActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDataActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetNewLevel(int32 NewLevel);
	const UDataTable* GetDataTable() const;
	UDataTable* GetDataTable();
	void SetDataTable(UDataTable* InDataTable);

	UFUNCTION(CallInEditor, Category = Info)
	void SetInfoData(int32 EditLevel);	

private:
	struct FCharInfoData* CurrentCharInfoData = nullptr;

	//UPROPERTY(EditAnywhere, AdvancedDisplay, Instanced, Category = DataTables)
	/*UPROPERTY()
	TArray<UDataTable*> DataTables;*/
	UPROPERTY(EditAnywhere, Category = DataTableList, meta = (AllowedClasses = "DataTable"))
	TArray<FSoftObjectPath> DataTableList;
	//TArray<UDataTable*> DataTableList;

	UPROPERTY(EditInstanceOnly, Category = Info, Meta = (AllowPrivateAccess = true))
	int32 Level;	

	/*UPROPERTY(Transient, VisibleInstanceOnly, Category = Info, Meta = (AllowPrivateAccess = true))
	float CurrentHP;*/

	UPROPERTY()
	class UDataTable* CharacterTable = nullptr;

	FCharInfoData* GetCharInfoData(int32 Level);
};
