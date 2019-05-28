// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class T1PROJECT_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void BeginDestroy() override;
		

private:
	void SetIcon();
	
	
public:
	UMyUserWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void OnDisplay();

	UFUNCTION(BlueprintCallable)
		void OnClickTest();

	UFUNCTION(BlueprintCallable)
		void OnClickText();
	


	UPROPERTY(EditAnywhere)
		FString _TempString;

private:
	int m_NumTest;
	

	
};
