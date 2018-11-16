// Fill out your copyright notice in the Description page of Project Settings.

#include "FMyExtensionCommands.h"
#include "FMyEditor.h"
#include "DataActorComponent.h"

#define LOCTEXT_NAMESPACE "MyCommand"

void FMyExtensionCommands::RegisterCommands()
{
	UI_COMMAND(Command1, "DataTableCenter", "DataTableCenter Tooltip", EUserInterfaceActionType::Button, FInputGesture());
}

void FMyExtensionActions::Action1()
{
	//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MyCommand", "My Editor Extension Action 1 Message"));	
	TSharedRef<FMyEditor> NewMyEditor(new FMyEditor());
	NewMyEditor->InitFMyEditor(EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), NewObject<UDataActorComponent>());
}

#undef LOCTEXT_NAMESPACE