// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../UI/GsUIIcon.h"
#include "CItem.h"
#include "ItemIconSelector.generated.h"


UCLASS()
class GAMESERVICE_API UItemIconSelector : public UObject
{
	GENERATED_BODY()

public:
	UItemIconSelector() = default;

public:
	enum eItemIconSize
	{
		Small = 0,
		Medium,
		Large,
		ExtraLarge,
	};

public:
	UGsUIIcon* CreateItemIcon(eItemIconSize In_eIconSize, UPanelWidget* In_parentWidget, const UWorld* In_worldObj);

	UGsUIIcon* SetItemIcon(UCItem* In_CItem);
private:
	const FString GetPathbyIconSize(eItemIconSize In_eSize);

private:
	eItemIconSize eIconSize = eItemIconSize::Large;
	UGsUIIcon* UIIcon;

};