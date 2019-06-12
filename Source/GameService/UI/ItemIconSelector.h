// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UIIcon.h"
#include "CItem.h"
#include "ItemIconSelector.generated.h"


UCLASS()
class GAMESERVICE_API UItemIconSelector : public UObject
{
	GENERATED_BODY()
	
public:
	UItemIconSelector() = default;

public:
	UUIIcon* CreateItemIcon();
	UUIIcon* SetItemIcon(const UCItem* In_CItem);
	
};