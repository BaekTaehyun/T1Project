// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Overlay.h"
#include "../../GameObject/Item/ItemIconSelector.h"
#include "GsUIHUDConsumable.generated.h"

/**
 *
 */
UCLASS()
class GAMESERVICE_API UGsUIHUDConsumable : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UOverlay* ItemImgRoot;

public:
	void CreateConsumeItem();
	void UpdateConsumeItem();

private:
	UItemIconSelector* ItemIconSelector;
};
