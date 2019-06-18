// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../GameObject/Item/CItem.h"
#include "Image.h"
#include "TextBlock.h"
#include "GsUIIcon.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsUIIcon : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItemCount(int32 In_count);
	void SetItem(UCItem* In_CItem);
	void SetDefaultItemImg();

public:
	virtual void NativeConstruct() override;

public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UImage* ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTextBlock* Item_CountText;

	//UFUNCTION(BlueprintCallable)
		//void ConstructEventCall();

	UFUNCTION(BlueprintCallable)
		void OnClickItemIconEvent();

private:
	UTexture2D* LoadTextureFromPath(const FString& Path);
	void TestLoadImgToTexture2D();

private:
	UCItem* Item;

};
