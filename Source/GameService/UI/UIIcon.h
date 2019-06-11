// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Image.h"
#include "TextBlock.h"
#include "UIIcon.generated.h"


/**
 * 
 */
UCLASS()
class GAMESERVICE_API UUIIcon : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUIIcon(const FObjectInitializer& ObjectInitializer);

public:
	void SetItemImg();

public:
	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UImage* ItemImg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTextBlock* Item_CountText;


	UFUNCTION(BlueprintCallable)
		void ConstructEventCall();

	UFUNCTION(BlueprintCallable)
		void OnClickItemIconEvent();

private:
	UTexture2D* LoadTextureFromPath(const FString& Path);
	void TestLoadImgToTexture2D();


};
