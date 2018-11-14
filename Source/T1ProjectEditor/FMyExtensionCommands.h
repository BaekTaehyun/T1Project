// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "FMyExtensionStyle.h"

class FMyExtensionCommands : public TCommands<FMyExtensionCommands>
{
public:
	FMyExtensionCommands()
		: TCommands<FMyExtensionCommands>(TEXT("MyExtensions"),
			NSLOCTEXT("MyExtensions", "MyExtensions", "MyExtensions"),
			NAME_None,
			//TEXT("Default"))^
			FMyExtensionStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > Command1;
	TSharedPtr< FUICommandInfo > Command2;
	TSharedPtr< FUICommandInfo > Command3;
	TSharedPtr< FUICommandInfo > Command4;
};

class FMyExtensionActions
{
public:
	static void Action1();
	static void Action2();
	static void Action3();
};